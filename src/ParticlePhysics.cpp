#include "ParticlePhysics.h"
#include <iostream>

auto IParticlePhysics::Make( ParticlePhysicsType type ) -> SharedParticlePhysics
{
	switch ( type )
	{
	case ParticlePhysicsType::xyFalling:
		return std::make_shared<ParticlePhysicsXYFalling>( );
	default:
		std::cout << "Unsupported particle physics type: " << type << std::endl;
		break;
	}

	return nullptr;
}

auto ParticlePhysicsXYFalling::InitRandom( ParticleState& data ) -> void
{
	// make random linear speeds
}

auto ParticlePhysicsXYFalling::ComputeMovement( TimePassedF timePassed,
												ParticleState& state, gen::Vec3& lastPos, gen::Vec3& newPos ) -> void
{
	// compute
}
