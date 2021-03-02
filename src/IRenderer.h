#pragma once

#include "ParticleGenConfig.h"
#include "IEffectManager.h"
#include <memory>

class IRenderer;
using SharedRenderer = std::shared_ptr<IRenderer>;

class IRenderer
{
public:
	virtual ~IRenderer( ) = default;
	virtual auto RenderFrame( ) -> void = 0;
	virtual auto OnWindowResized( int width, int height ) -> void = 0;

	static auto Make( SharedEffectManager dataModel ) -> SharedRenderer;
};
