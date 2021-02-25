#pragma once

#include "ParticleGenConfig.h"
#include "IRenderer.h"

class IWindowManager;
using UniqueWindowManager = std::unique_ptr<IWindowManager>;

class IWindowManager
{
public:
	virtual ~IWindowManager( ) = default;
	virtual auto StartMainLoop( ) -> void = 0;

	static auto Make( int argc, char** argv, SharedRenderer renderer ) -> UniqueWindowManager;
};
