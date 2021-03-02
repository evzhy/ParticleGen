#pragma once

#include "IEffectManager.h"
#include <mutex>

struct GenerationSyncState
{
	struct SyncStateThreadData
	{
		EffectQueue effectQueue;
		std::size_t particlesActive{0};  // alive particles in thread, updated by thread on sync
		std::size_t particlesInQueue{0}; // in queue for thread, updated by sync on adding new effects from any thread
		std::shared_ptr<std::atomic<bool>> effectQueueFlag;

		SyncStateThreadData( );
	};

	std::vector<SyncStateThreadData> threadData;
	std::mutex threadBalancingMutex; // lock/unlock mutex during UpdateEffectsForThread

	GenerationSyncState( );

	GenerationSyncState( const GenerationSyncState& other ) = delete;
	GenerationSyncState( const GenerationSyncState&& other ) = delete;
	void operator=( const GenerationSyncState& other ) = delete;
	void operator=( const GenerationSyncState&& other ) = delete;
};

class GenerationSync : public IGenerationSync
{
public:
	explicit GenerationSync( GenerationSyncState& state );
	~GenerationSync( ) override;

	auto CreateEffectAtPos( gen::Vec3 pos ) -> void override;
	auto UpdateEffectsForThread( std::size_t threadIndex, std::size_t activeParticlesInThisThread,
								 std::vector<gen::Vec3>& explodedParticleCoords ) -> EffectQueue override;

private:
	GenerationSyncState& mState;
	EffectQueue mDummy;
};
