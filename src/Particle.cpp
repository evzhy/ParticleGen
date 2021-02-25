#include "Particle.h"

Particle::Particle( SharedParticlePhysics physics, gen::Vec3& lastPos, gen::Vec3& newPos )
	: mPhysics( physics ), mLastPos( lastPos ), mNewPos( newPos )
{
	if ( mPhysics != nullptr )
	{
		mPhysics->InitRandom( mState );
	}

	// TODO: move all chance/effect logic to some strategy pattern
	// TODO: use <random> with better distribution
	float percentRoll = static_cast<float>( rand( ) ) / RAND_MAX * 100.f;
	mCreatesNewEffect = percentRoll <= gParticleExplodePercent;
}

auto Particle::Reset( gen::Vec3& lastPos, gen::Vec3& newPos, TimePassedF lifetime ) -> void
{
	mLastPos = lastPos;
	mNewPos = newPos;

	mState.Reset( );
	mState.lifetime = lifetime;
	mState.active = true;
}

auto Particle::SwitchPosRefs( gen::Vec3& newPos ) -> void
{
	mLastPos = mNewPos;
	mNewPos = newPos;
}

auto Particle::Tick( TimePassedF timePassed ) -> bool
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
		mPhysics->ComputeMovement( timePassed, mState, mLastPos, mNewPos ); // compute only for
	}

	return false;
}

auto Particle::GetPos( ) const -> gen::Vec3
{
	return mLastPos;
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
	return mCreatesNewEffect;
}