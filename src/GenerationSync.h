#pragma once

#include "IEffectManager.h"
#include <mutex>

class GenerationSync : public IGenerationSync
{
public:
	explicit GenerationSync( std::mutex& threadBalancingMutex, std::atomic<bool>& newEffectFlag,
							 std::size_t numberOfThreads );
	~GenerationSync( ) override;

	auto CreateEffectAtPos( gen::Vec3 pos ) -> void override;
	auto UpdateEffectsForThread( std::size_t threadIndex, std::size_t activeParticlesInThisThread,
								 std::vector<gen::Vec3>& explodedParticleCoords ) -> EffectQueue& override;

private:
	std::mutex& mThreadBalancingMutex; // lock/unlock mutex on ctor/dtor
	std::atomic<bool>& mNewEffectsFlag;
	std::size_t mNumberOfThreads{0};
	std::vector<EffectQueue> mEffectQueues;
	std::vector<std::size_t> mThreadActiveParticleCount; // alive particles per thread

	EffectQueue mDummy;
};
