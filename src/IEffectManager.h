#pragma once

#include "IRenderData.h"

#include <memory>
#include <atomic>

// coordinate of new effect and number of particles in it
using EffectScenario = std::pair<gen::Vec3, std::size_t>;
using EffectQueue = std::vector<EffectScenario>;

class IGenerationSync
{
public:
	virtual ~IGenerationSync( ) = default;

	virtual auto CreateEffectAtPos( gen::Vec3 pos ) -> void = 0;

	/* thread synchronization - updates active particle count for thread and new effect coords
	   returns array of effects with particle count */
	virtual auto UpdateEffectsForThread( std::size_t threadIndex, std::size_t activeParticlesInThisThread,
										 std::vector<gen::Vec3>& explodedParticleCoords ) -> EffectQueue& = 0;
};

class IEffectManager;
using SharedEffectManager = std::shared_ptr<IEffectManager>;

class IEffectManager
{
public:
	virtual ~IEffectManager( ) = default;

	static auto Make( ) -> SharedEffectManager;

	virtual auto GetRenderData( ) -> std::unique_ptr<IRenderData> = 0;		   // requested from main thread
	virtual auto GetGenerationSync( ) -> std::unique_ptr<IGenerationSync> = 0; // requrested from generation threads
};
