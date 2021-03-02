#pragma once

#include "IParticlePhysics.h"

class Particle final // if inherited, remove final and make destructor virtual
{
public:
	explicit Particle( SharedParticlePhysics physics, gen::Vec3& lastPos, gen::Vec3& newPos );
	~Particle( ) = default;

	auto Reset( gen::Vec3& newPos ) -> void;
	auto SwitchPosRefs( gen::Vec3& newPos ) -> void;
	auto SetAlive( bool alive ) -> void;

	auto Tick( TimePassedF timePassed ) -> bool; // returns true if was alive and expired this tick
	auto GetPos( ) const -> gen::Vec3;
	auto IsActive( ) const -> bool;
	auto CreatesNewEffect( ) const -> bool;

private:
	gen::Vec3& mLastPos;
	gen::Vec3& mNewPos;
	SharedParticlePhysics mPhysics;
	ParticleState mState;

	bool mCreatesNewEffect{false};
};
