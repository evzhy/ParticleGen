#pragma once

#include "IEffectManager.h"
#include <mutex>

struct GenerationSyncState
{
	std::vector<EffectQueue> effectQueues;
	std::vector<std::size_t> particlesCount; // alive particles per thread

	std::mutex& threadBalancingMutex; // lock/unlock mutex on ctor/dtor
	std::atomic<bool>& newEffectsFlag;

	GenerationSyncState( std::mutex& threadBalancingMutex, std::atomic<bool>& newEffectsFlag );
};

class GenerationSync : public IGenerationSync
{
public:
	explicit GenerationSync( GenerationSyncState& state );
	~GenerationSync( ) override;

	auto CreateEffectAtPos( gen::Vec3 pos ) -> void override;
	auto UpdateEffectsForThread( std::size_t threadIndex, std::size_t activeParticlesInThisThread,
								 std::vector<gen::Vec3>& explodedParticleCoords ) -> EffectQueue& override;

private:
	GenerationSyncState& mState;
	EffectQueue mDummy;
};
