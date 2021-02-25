#include "RenderData.h"

#ifdef _DEBUG
#include <assert.h>
#endif

RenderData::RenderData( GeneratorVec& threadData, std::condition_variable& afterRenderNotifier,
						std::atomic<bool>& renderingInProgress )
	: mThreadData( threadData ), mAfterRenderNotifier( afterRenderNotifier ), mRenderingInProgress( renderingInProgress )
{
	renderingInProgress.store( true );
}

RenderData::~RenderData( )
{
	mRenderingInProgress.store( false );
	mAfterRenderNotifier.notify_all( );
}

auto RenderData::GetDataBuffersCount( ) -> std::size_t
{
	return mThreadData.size( );
}

auto RenderData::GetParticleData( std::size_t index ) -> ParticleData&
{
	auto threadsCount = mThreadData.size( );

	if ( index >= threadsCount )
	{
		return mDummy;
	}

	auto ptr = mThreadData[index];
#ifdef _DEBUG
	assert( ptr != nullptr );
#endif
	auto& threadData = *ptr;

	return threadData.firstIsPassive.load( ) ? threadData.first : threadData.second;
}
