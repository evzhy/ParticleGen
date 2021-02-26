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
	auto CheckNewEffectsInQueueForThread( std::size_t threadIndex ) -> bool;

private:
	std::atomic<bool> mWorking{false};

	std::vector<std::thread> mThreads;
	GeneratorVec mThreadData;

	std::condition_variable mAfterRenderNotifier;
	std::mutex mAfterRenderMutex;
	std::atomic<bool> mRenderingInProgress{false};

	GenerationSyncState mSyncState;
};
