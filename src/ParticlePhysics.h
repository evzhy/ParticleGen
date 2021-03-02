#pragma once

#include "IParticlePhysics.h"

class ParticlePhysicsXYFalling : public IParticlePhysics
{
public:
	explicit ParticlePhysicsXYFalling( ) = default;
	~ParticlePhysicsXYFalling( ) override = default;

	auto InitRandom( ParticleState& data ) -> void override;
	auto ComputeMovement( ParticleState& state, gen::Vec3& computedPos ) -> void override;
};
