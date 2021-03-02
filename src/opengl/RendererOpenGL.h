#pragma once

#include "../IRenderer.h"

class RendererOpenGL : public IRenderer
{
public:
	explicit RendererOpenGL( SharedEffectManager effectManager );
	~RendererOpenGL( ) override;

	auto RenderFrame( ) -> void override;
	auto OnWindowResized( int width, int height ) -> void override;

private:
	SharedEffectManager mEffectManager;
};
