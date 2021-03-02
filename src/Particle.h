#pragma once

#include "IParticlePhysics.h"

class Particle final // if inherited, remove final and make destructor virtual
{
public:
	explicit Particle( SharedParticlePhysics physics );
	~Particle( ) = default;

	auto Reset( const gen::Vec3& startPos ) -> void;
	auto SetAlive( bool alive ) -> void;

	auto Tick( TimeDiff timePassed, gen::Vec3& writePos ) -> bool; // returns true if was alive and expired this tick
	auto IsActive( ) const -> bool;
	auto CreatesNewEffect( ) const -> bool;

private:
	SharedParticlePhysics mPhysics;
	ParticleState mState;
};
