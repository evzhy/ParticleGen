#include "ParticlePhysics.h"
#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

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

auto ParticlePhysicsXYFalling::InitRandom( ParticleState& state ) -> void
{
	static const float fPI = static_cast<float>( M_PI );

	// make random linear speeds
	float angle = static_cast<float>( rand( ) ) / static_cast<float>( RAND_MAX ) * 360.f;
	float initialLinearSpeed = 0.1f; // TODO: replace with global
	float angleRadian = angle * fPI / 180.f;
	state.linearSpeeds.x = initialLinearSpeed * cos( angleRadian );
	state.linearSpeeds.y = initialLinearSpeed * sin( angleRadian );

	// set random lifetime
	state.lifetime = TimePassedF( 11.f );
}

auto ParticlePhysicsXYFalling::ComputeMovement( TimePassedF timePassed,
												ParticleState& state, gen::Vec3& lastPos, gen::Vec3& newPos ) -> void
{
	auto time = timePassed.count( );
	newPos.x = lastPos.x + state.linearSpeeds.x * time;
	newPos.y = lastPos.y + state.linearSpeeds.y * time;
}
