#pragma once

#include "../src/ParticleGenConfig.h"

#ifdef IMPL_GLUT

#include "../IWindowManager.h"

class ManagerGLUT : public IWindowManager
{
public:
	explicit ManagerGLUT( int argc, char** argv, SharedRenderer renderer );
	~ManagerGLUT( ) override;

	auto StartMainLoop( ) -> void override;

private:
	SharedRenderer mRenderer;
};

#endif
