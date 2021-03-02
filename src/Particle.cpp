#include "Particle.h"

Particle::Particle( SharedParticlePhysics physics )
	: mPhysics( physics )
{
	if ( mPhysics != nullptr )
	{
		mPhysics->InitRandom( mState );
	}
}

auto Particle::Reset( const gen::Vec3& startPos ) -> void
{
	mState.Reset( );
	mState.startPos = startPos;
	mState.active = true;

	if ( mPhysics != nullptr )
	{
		mPhysics->InitRandom( mState );
	}
}

auto Particle::Tick( TimeDiff timePassed, gen::Vec3& writePos ) -> bool
{
	if ( !mState.active )
	{
		return false;
	}

	mState.age += timePassed;

	if ( mState.age >= mState.lifetime )
	{
		mState.active = false;
		return true;
	}

	if ( mPhysics != nullptr )
	{
		mPhysics->ComputeMovement( mState, writePos );
	}

	return false;
}

auto Particle::SetAlive( bool alive ) -> void
{
	mState.active = alive;
}

auto Particle::IsActive( ) const -> bool
{
	return mState.active;
}

auto Particle::CreatesNewEffect( ) const -> bool
{
	return mState.isSpecial;
}