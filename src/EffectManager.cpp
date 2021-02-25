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

EffectManager::EffectManager( )
	: mSyncState( mThreadBalancingMutex, mNewEffectsInSync )
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
	return std::move( std::make_unique<GenerationSync>( mSyncState ) );
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
	// init particles list
	std::list<Particle> particles;
	auto particlePhysics = IParticlePhysics::Make( ParticlePhysicsType::xyFalling );

	for ( std::size_t i = 0; i < data.first.data.size( ); ++i )
	{
		particles.emplace_back( particlePhysics, data.first.data[i], data.second.data[i] );
	}

	std::vector<gen::Vec3> explodedParticles;

	auto threadStartTime = std::chrono::system_clock::now( );
	auto lastCycleTime = threadStartTime;
	auto currentTime = lastCycleTime;

	bool firstToSecondRW = data.firstIsPassive.load( ); // from first buffer (read) to second (write)

	while ( mWorking.load( ) )
	{
		explodedParticles.clear( );

		lastCycleTime = currentTime;
		currentTime = std::chrono::system_clock::now( );
		std::chrono::duration<float> timeDiff = currentTime - lastCycleTime;

		// calc particle coords ------------------------------------------------------------------------
		std::size_t writeBufferIndex = 0; // index of current active vertex in active (write) buffer
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
					explodedParticles.push_back( particle.GetPos( ) );
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

		// sync - update number of active particles -----------------------------------------------------
		if ( !explodedParticles.empty( ) || mNewEffectsInSync.load( ) )
		{
			auto sync = GetGenerationSync( );
			auto& newEffects = sync->UpdateEffectsForThread( threadIndex, writeBufferIndex, explodedParticles );

			if ( !newEffects.empty( ) )
			{
				//
			}
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
