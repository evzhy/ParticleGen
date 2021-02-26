#include "GenerationSync.h"
#include "ParticleGenConfig.h"

GenerationSyncState::SyncStateThreadData::SyncStateThreadData( )
	: effectQueueFlag( std::make_shared<std::atomic<bool>>( false ) )
{
}

GenerationSyncState::GenerationSyncState( )
{
	threadData.resize( gNumberOfGenerationThreads );
}

//-----------------------------------------------------------------------

GenerationSync::GenerationSync( GenerationSyncState& state )
	: mState( state )
{
}

GenerationSync::~GenerationSync( )
{
}

auto GenerationSync::CreateEffectAtPos( gen::Vec3 pos ) -> void
{
}

auto GenerationSync::UpdateEffectsForThread( std::size_t threadIndex, std::size_t activeParticlesInThisThread,
											 std::vector<gen::Vec3>& explodedParticleCoords ) -> EffectQueue&&
{
	if ( threadIndex < gNumberOfGenerationThreads )
	{
		std::lock_guard<std::mutex> lk( mState.threadBalancingMutex );

		auto& data = mState.threadData[threadIndex];
		data.particlesActive = activeParticlesInThisThread;

		// update particle counters
		std::size_t totalParticles = 0;
		std::size_t cycledThreadIndex = 0;

		for ( auto& data : mState.threadData )
		{
			totalParticles += data.particlesActive + data.particlesInQueue;
		}

		if ( totalParticles < gMaxParticleTotal )
		{
			// under particle limit, can generate new effects
		}

		auto&& queue = std::move( data.effectQueue );
		data.effectQueue = {};
		data.effectQueueFlag->store( false );

		return std::move( queue );
	}

	return std::move( mDummy );
}
