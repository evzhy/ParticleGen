#include "ParticleGenConfig.h"
#include "IEffectManager.h"
#include "IWindowManager.h"

#include <iostream>

int main( int argc, char** argv )
{
	auto particleManager = IEffectManager::Make( );
	auto renderer = IRenderer::Make( particleManager );
	auto windowManager = IWindowManager::Make( argc, argv, renderer );

	if ( particleManager == nullptr || renderer == nullptr || windowManager == nullptr )
	{
		std::cout << "Initialization error" << std::endl;
		return -1;
	}

	windowManager->StartMainLoop( );

	return 0;
}
