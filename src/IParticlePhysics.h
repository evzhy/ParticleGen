#pragma once

#include "ParticleGenConfig.h"
#include "ParticleGenTypes.h"

#include <memory>
#include <chrono>

//---------------------------------------------------------------------

using TimePassedF = std::chrono::duration<float>;

typedef struct
{
	gen::Vec3 linearSpeeds;
	TimePassedF age{0.f};	  // time since creation
	TimePassedF lifetime{0.f}; // age limit, set active to false when reached, stop ticking
	float linearDeccelerationCumulativePercent{0.f};
	float fallSpeed{0.f};
	bool active{false}; // !expired

	auto Reset( ) -> void
	{
		linearSpeeds = {0.f, 0.f, 0.f};
		age = TimePassedF( 0.f );
		lifetime = TimePassedF( 0.f );
		linearDeccelerationCumulativePercent = 0.f;
		fallSpeed = 0.f;
		active = false;
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
	virtual auto ComputeMovement( TimePassedF timePassed,
								  ParticleState& state, gen::Vec3& lastPos, gen::Vec3& newPos ) -> void = 0;

	static auto Make( ParticlePhysicsType type ) -> SharedParticlePhysics;
};
