#pragma once

#include "IEffectManager.h"
#include "RenderData.h"

// lock/unlock mutex on ctor/dtor
class GenerationSync : public IGenerationSync
{
public:
	explicit GenerationSync( std::mutex& threadBalancingMutex, std::size_t numberOfThreads );
	~GenerationSync( ) override;

	auto CreateEffectAtPos( gen::Vec3 pos ) -> void override;
	auto GetEffectQueueForThread( std::size_t threadIndex ) -> EffectQueue& override;

private:
	std::mutex& mThreadBalancingMutex;
	std::size_t mNumberOfThreads{0};
	std::vector<EffectQueue> mEffectQueues;

	EffectQueue mDummy;
};

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
};
