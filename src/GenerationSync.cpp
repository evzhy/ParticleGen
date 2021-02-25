#include "GenerationSync.h"
#include "ParticleGenConfig.h"

GenerationSyncState::GenerationSyncState( std::mutex& threadBalancingMutex, std::atomic<bool>& newEffectsFlag )
	: threadBalancingMutex( threadBalancingMutex ), newEffectsFlag( newEffectsFlag )
{
	effectQueues.resize( gNumberOfGenerationThreads );
	particlesCount.resize( gNumberOfGenerationThreads );
}

//-----------------------------------------------------------------------

GenerationSync::GenerationSync( GenerationSyncState& state )
	: mState( state )
{
	mState.threadBalancingMutex.lock( );
}

GenerationSync::~GenerationSync( )
{
	mState.threadBalancingMutex.unlock( );
}

auto GenerationSync::CreateEffectAtPos( gen::Vec3 pos ) -> void
{
}

auto GenerationSync::UpdateEffectsForThread( std::size_t threadIndex, std::size_t activeParticlesInThisThread,
											 std::vector<gen::Vec3>& explodedParticleCoords ) -> EffectQueue&
{
	if ( threadIndex < gNumberOfGenerationThreads )
	{
		// update particle counters
		mState.particlesCount[threadIndex] = activeParticlesInThisThread;
		std::size_t totalParticles = 0;

		for ( auto particlesInThread : mState.particlesCount )
		{
			totalParticles += particlesInThread;
		}

		for ( auto& newEffectsThreadQueue : mState.effectQueues )
		{
			for ( auto& newEffectScenario : newEffectsThreadQueue )
			{
				totalParticles += newEffectScenario.second;
			}
		}

		static const auto maxParticles = gMaxParticleEffects * gMaxParticlesPerEffect;

		if ( totalParticles < maxParticles )
		{
			// under particle limit, can generate new effects

			mState.newEffectsFlag.store( true );
		}

		return mState.effectQueues[threadIndex];
	}

	return mDummy;
}
