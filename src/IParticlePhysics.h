#pragma once

#include "ParticleGenConfig.h"
#include "ParticleGenTypes.h"

#include <memory>
#include <chrono>

//---------------------------------------------------------------------

using TimeDiff = decltype( std::chrono::system_clock::now( ) - std::chrono::system_clock::now( ) );

typedef struct
{
	gen::Vec3 startPos; // initial position
	gen::Vec3 linearSpeeds;
	TimeDiff age{0};	   // time since creation
	TimeDiff lifetime{0};  // age limit, set active to false when reached, stop ticking
	bool active{false};	// !expired
	bool isSpecial{false}; // creates new effect when expired

	auto Reset( ) -> void
	{
		startPos = {0.f, 0.f, 0.f};
		linearSpeeds = {0.f, 0.f, 0.f};
		age = TimeDiff( 0 );
		lifetime = TimeDiff( 0 );
		active = false;
		isSpecial = false;
	}
} ParticleState;

//---------------------------------------------------------------------

enum ParticlePhysicsType
{
	xyFalling
};

class IParticlePhysics;
using SharedParticlePhysics = std::shared_ptr<IParticlePhysics>;

class IParticlePhysics
{
public:
	virtual ~IParticlePhysics( ) = default;

	virtual auto InitRandom( ParticleState& state ) -> void = 0;
	virtual auto ComputeMovement( ParticleState& state, gen::Vec3& computedPos ) -> void = 0;

	static auto Make( ParticlePhysicsType type ) -> SharedParticlePhysics;
};
