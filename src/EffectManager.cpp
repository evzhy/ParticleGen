#include "EffectManager.h"
#include "ParticleGenConfig.h"
#include "Particle.h"

#include <iostream>
#include <list>
#include <chrono>

auto IEffectManager::Make( ) -> SharedEffectManager
{
	return std::make_shared<EffectManager>( );
}

//-----------------------------------------------------------

GenerationSync::GenerationSync( std::mutex& threadBalancingMutex, std::size_t numberOfThreads )
	: mThreadBalancingMutex( threadBalancingMutex ), mNumberOfThreads( numberOfThreads )
{
	mThreadBalancingMutex.lock( );
}

GenerationSync::~GenerationSync( )
{
	mThreadBalancingMutex.unlock( );
}

auto GenerationSync::CreateEffectAtPos( gen::Vec3 pos ) -> void
{
	// think if use effects (same lifetime of all particles) or just particles
}

auto GenerationSync::GetEffectQueueForThread( std::size_t threadIndex ) -> EffectQueue&
{
	return threadIndex < mNumberOfThreads ? mEffectQueues[threadIndex] : mDummy;
}

//-----------------------------------------------------------

EffectManager::EffectManager( )
{
	Init( );
}

EffectManager::~EffectManager( )
{
	mWorking.store( false );
	mAfterRenderNotifier.notify_all( ); // notify all threads that may be waiting for rendering to finish

	for ( auto& mThread : mThreads )
	{
		mThread.join( );
	}
}

auto EffectManager::GetRenderData( ) -> std::unique_ptr<IRenderData>
{
	return std::move( std::make_unique<RenderData>( mThreadData, mAfterRenderNotifier, mRenderingInProgress ) );
}

auto EffectManager::GetGenerationSync( ) -> std::unique_ptr<IGenerationSync>
{
	return std::move( std::make_unique<GenerationSync>( mThreadBalancingMutex, mThreadData.size( ) ) );
}

auto EffectManager::Init( ) -> void
{
	if ( mWorking.load( ) )
	{
		std::cout << "Trying to initialize Effect Manager again?" << std::endl;
		return;
	}

	const std::size_t maxBufferSize = gMaxParticleEffects * gMaxParticlesPerEffect;

	// fill threads data
	for ( int i = 0; i < gNumberOfGenerationThreads; ++i )
	{
		std::size_t bufferSize = maxBufferSize / gNumberOfGenerationThreads;

		if ( i == 0 )
		{
			bufferSize += maxBufferSize % gNumberOfGenerationThreads;
		}

		mThreadData.push_back( std::make_shared<ThreadGeneratorData>( bufferSize ) );
	}

	// make first effect
	{
		auto sync = GetGenerationSync( );
		sync->CreateEffectAtPos( {0.f, 0.f, 0.f} );
	}

	// run threads
	for ( std::size_t i = 0; i < gNumberOfGenerationThreads; ++i )
	{
		auto ptr = mThreadData[i];

		if ( ptr == nullptr )
		{
			std::cout << "Nullptr in shared pointer of thread data at " << i << std::endl;
			continue;
		}

		mThreads.emplace_back( &EffectManager::GenerationThreadCycle, this, std::ref( *ptr ), i );
	}
}

auto EffectManager::GenerationThreadCycle( ThreadGeneratorData& data, std::size_t threadIndex ) -> void
{
	// list of Particle, each has ref to vec3 in stable array and gets vec3 into active array
	// Particle needs an Init that changes refs every time

	// init particles list
	std::list<Particle> particles;
	auto particlePhysics = IParticlePhysics::Make( ParticlePhysicsType::xyFalling );

	for ( std::size_t i = 0; i < data.first.data.size( ); ++i )
	{
		particles.emplace_back( particlePhysics, data.first.data[i], data.second.data[i] );
	}

	auto threadStartTime = std::chrono::system_clock::now( );
	auto lastCycleTime = threadStartTime;
	auto currentTime = lastCycleTime;

	bool firstToSecondRW = data.firstIsPassive.load( ); // from first buffer (read) to second (write)

	while ( mWorking.load( ) )
	{
		lastCycleTime = currentTime;
		currentTime = std::chrono::system_clock::now( );
		std::chrono::duration<float> timeDiff = currentTime - lastCycleTime;

		// calc particle coords
		int writeBufferIndex = 0; // index of current active vertex in active (write) buffer
		auto& writeBuffer = firstToSecondRW ? data.second.data : data.first.data;
		auto particleIterator = particles.begin( );

		for ( ; particleIterator != particles.end( ); ++particleIterator )
		{
			auto& particle = *particleIterator;

			if ( !particle.IsActive( ) )
			{
				break; // reaching expired particle means end of processed particles
			}

			particle.SwitchPosRefs( writeBuffer[writeBufferIndex] );
			auto particleExpired = particle.Tick( timeDiff );

			if ( particleExpired )
			{
				// move to end of list, if exploded into new effect, add to new effects array
				if ( particle.CreatesNewEffect( ) )
				{
					auto newEffectPosition = particle.GetPos( );
					// TODO: add new effect to array
				}

				auto nextIterator = particleIterator;
				++nextIterator;

				particles.splice( particles.end( ), particles, particleIterator );
				particleIterator = --nextIterator;
			}
			else
			{
				++writeBufferIndex;
			}
		}

		// sync - update number of active particles
		{
			auto sync = GetGenerationSync( );
			//sync->UpdateEffectsForThread( threadIndex, newEffectPosVec )
			// update sync with number of vertices in this thread
			// send particles-becoming-effects to sync, let it calc the load
			// get new effects with particles from sync
		}

		// update new number of active particles for rendering
		auto& useCounter = firstToSecondRW ? data.second.usedVerticesCount : data.first.usedVerticesCount;
		useCounter = writeBufferIndex;

		firstToSecondRW = !firstToSecondRW; // switch buffer direction

		if ( mRenderingInProgress.load( ) )
		{
			std::unique_lock<std::mutex> lk( mAfterRenderMutex );

			if ( mRenderingInProgress.load( ) )
			{
				mAfterRenderNotifier.wait( lk );
			}
		}

		// switch buffers immediately after optional waiting for the end of rendering
		data.firstIsPassive.store( firstToSecondRW );
	}
}
