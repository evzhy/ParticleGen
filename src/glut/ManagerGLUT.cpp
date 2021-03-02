#include "ManagerGLUT.h"
#include <thread>

#ifdef IMPL_GLUT

#include <GL/glut.h>

auto IWindowManager::Make( int argc, char** argv, SharedRenderer renderer ) -> UniqueWindowManager
{
	return std::make_unique<ManagerGLUT>( argc, argv, renderer );
}

// ---------- todo: make this less bad --------------------
static SharedRenderer staticRendererPointer = nullptr;

void CStyleRenderCallback( )
{
	if ( staticRendererPointer != nullptr )
	{
		staticRendererPointer->RenderFrame( );
		glutSwapBuffers( );
	}
}

void CStyleReshapeCallback( int width, int height )
{
	if ( staticRendererPointer )
	{
		staticRendererPointer->OnWindowResized( width, height );
	}
}

void CStyleIdleCallback( )
{
	std::this_thread::sleep_for( std::chrono::milliseconds( gRedrawInterval ) );
	glutPostRedisplay( );
}
// -------------------------------------------------------

ManagerGLUT::ManagerGLUT( int argc, char** argv, SharedRenderer renderer )
	: mRenderer( renderer )
{
	staticRendererPointer = renderer;

	glutInit( &argc, argv );
	glutInitWindowSize( gWindowWidth, gWindowHeight );
	glutCreateWindow( gWindowTitle );
	glutInitDisplayMode( GLUT_DOUBLE );
	glutDisplayFunc( CStyleRenderCallback );
	glutReshapeFunc( CStyleReshapeCallback );
	glutIdleFunc( CStyleIdleCallback );
}

ManagerGLUT::~ManagerGLUT( )
{
	staticRendererPointer = nullptr;
}

auto ManagerGLUT::StartMainLoop( ) -> void
{
	glutMainLoop( );
}

#endif
