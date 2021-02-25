#include "GenerationSync.h"

#ifdef _DEBUG
#include <assert.h>
#endif

GenerationSync::GenerationSync( std::mutex& threadBalancingMutex, std::atomic<bool>& newEffectFlag,
								std::size_t numberOfThreads )
	: mThreadBalancingMutex( threadBalancingMutex ), mNewEffectsFlag( newEffectFlag ), mNumberOfThreads( numberOfThreads )
{
#ifdef _DEBUG
	assert( mNumberOfThreads > 0 );
#endif

	mEffectQueues.resize( mNumberOfThreads );
	mEffectQueues.resize( mNumberOfThreads );

	mThreadBalancingMutex.lock( );
}

GenerationSync::~GenerationSync( )
{
	mThreadBalancingMutex.unlock( );
}

auto GenerationSync::CreateEffectAtPos( gen::Vec3 pos ) -> void
{
	// TODO: balance threads
}

auto GenerationSync::UpdateEffectsForThread( std::size_t threadIndex, std::size_t activeParticlesInThisThread,
											 std::vector<gen::Vec3>& explodedParticleCoords ) -> EffectQueue&
{
	if ( threadIndex < mNumberOfThreads )
	{
		// update number of active particles per thread

		return mEffectQueues[threadIndex];
	}

	return mDummy;
}
