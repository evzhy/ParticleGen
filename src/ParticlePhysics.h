#pragma once

#include "IParticlePhysics.h"

class ParticlePhysicsXYFalling : public IParticlePhysics
{
public:
	explicit ParticlePhysicsXYFalling( ) = default;
	~ParticlePhysicsXYFalling( ) override = default;

	auto InitRandom( ParticleState& data ) -> void override;
	auto ComputeMovement( TimePassedF timePassed,
						  ParticleState& state, gen::Vec3& lastPos, gen::Vec3& newPos ) -> void override;
};
