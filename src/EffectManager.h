#pragma once

#include "IEffectManager.h"
#include "RenderData.h"
#include "GenerationSync.h"

class EffectManager : public IEffectManager
{
public:
	explicit EffectManager( );
	~EffectManager( ) override;

	auto GetRenderData( ) -> std::unique_ptr<IRenderData> override;
	auto GetGenerationSync( ) -> std::unique_ptr<IGenerationSync> override;

private:
	auto Init( ) -> void;
	auto GenerationThreadCycle( ThreadGeneratorData& data, std::size_t threadIndex ) -> void;

private:
	std::atomic<bool> mWorking{true};

	std::vector<std::thread> mThreads;
	GeneratorVec mThreadData;

	std::condition_variable mAfterRenderNotifier;
	std::mutex mAfterRenderMutex;
	std::mutex mThreadBalancingMutex;
	std::atomic<bool> mRenderingInProgress{false};

	// flag for synchronizing generation threads to get new effects
	// - set to true from GenerationSync, set to false during synchronization after getting new effect array
	std::atomic<bool> mNewEffectsInSync{false};
};
