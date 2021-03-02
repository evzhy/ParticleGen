#pragma once

#include "IRenderData.h"

#include <atomic>
#include <mutex>
#include <condition_variable>

class ThreadGeneratorData
{
public:
	ParticleData first;
	ParticleData second;

	// atomic flag for switching buffer pair: passive for rendering, active for modification
	std::atomic<bool> firstIsPassive{true};

	ThreadGeneratorData( std::size_t bufferSize )
		: first( ParticleData::Make( bufferSize ) ), second( ParticleData::Make( bufferSize ) )
	{
	}

	ThreadGeneratorData( const ThreadGeneratorData& other ) = delete;
	ThreadGeneratorData( const ThreadGeneratorData&& other ) = delete;
	ThreadGeneratorData& operator=( const ThreadGeneratorData& other ) = delete;
	ThreadGeneratorData& operator=( const ThreadGeneratorData&& other ) = delete;
};

using GeneratorVec = std::vector<std::shared_ptr<ThreadGeneratorData>>; // have to wrap in shared_ptr because unmovable atomic

//--------------------------------------------

class RenderData : public IRenderData
{
public:
	explicit RenderData( GeneratorVec& threadData, std::condition_variable& afterRenderNotifier,
						 std::atomic<bool>& renderingInProgress );
	~RenderData( ) override;

	auto GetDataBuffersCount( ) -> std::size_t override;
	auto GetParticleData( std::size_t index ) -> ParticleData& override;

private:
	GeneratorVec& mThreadData;
	std::condition_variable& mAfterRenderNotifier;
	std::atomic<bool>& mRenderingInProgress;

	ParticleData mDummy{std::move( ParticleData::Make( 0 ) )};
};
