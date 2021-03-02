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
	float initialLinearSpeed = gParticleInitialSpeed;
	float angleRadian = angle * fPI / 180.f;
	state.linearSpeeds.x = initialLinearSpeed * cos( angleRadian );
	state.linearSpeeds.y = initialLinearSpeed * sin( angleRadian );

	// set random lifetime
	state.lifetime = std::chrono::milliseconds( gParticleLifetimeMS );

	// use random and explosion chance to set explode flag
	state.isSpecial = static_cast<float>( rand( ) ) / static_cast<float>( RAND_MAX ) * 100.f <= gParticleExplodePercent;
}

auto ParticlePhysicsXYFalling::ComputeMovement( ParticleState& state, gen::Vec3& computedPos ) -> void
{
	auto time = static_cast<float>( std::chrono::duration_cast<std::chrono::milliseconds>( state.age ).count( ) ) / 1000.f;

	computedPos.x = state.startPos.x + state.linearSpeeds.x * time;
	computedPos.y = state.startPos.y + state.linearSpeeds.y * time;
}
