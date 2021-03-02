#include "RendererOpenGL.h"

#ifdef IMPL_OPENGL

#ifdef _WIN32
#define no_init_all deprecated
#include "windows.h"
#endif

#include <gl/GL.h>

auto IRenderer::Make( SharedEffectManager effectManager ) -> SharedRenderer
{
	return std::make_shared<RendererOpenGL>( effectManager );
}

RendererOpenGL::RendererOpenGL( SharedEffectManager effectManager )
	: mEffectManager( effectManager )
{
}

RendererOpenGL::~RendererOpenGL( )
{
}

auto RendererOpenGL::RenderFrame( ) -> void
{
	glClear( GL_COLOR_BUFFER_BIT );
	glColor3f( 1.0f, 1.0f, 1.0f );

	{
		glEnableClientState( GL_VERTEX_ARRAY );

		auto renderData = mEffectManager->GetRenderData( );
		auto bufferCount = renderData->GetDataBuffersCount( );

		for ( decltype( bufferCount ) i = 0; i < bufferCount; ++i )
		{
			auto& particleData = renderData->GetParticleData( i );

			if ( particleData.usedVerticesCount < 1 )
			{
				continue;
			}

			glVertexPointer( coordinatesPerVertex, GL_FLOAT, 0, particleData.data.data( ) );
			glDrawArrays( GL_POINTS, 0, static_cast<GLsizei>( particleData.usedVerticesCount ) );
		}

		glDisableClientState( GL_VERTEX_ARRAY );
	}

	glFlush( );
}

auto RendererOpenGL::OnWindowResized( int width, int height ) -> void
{
	glMatrixMode( GL_PROJECTION ); // Select The Projection Stack
	glLoadIdentity( );
	glOrtho( -width / 2, width / 2, -height / 2, height / 2, -1.0, 1.0 );

	glViewport( 0, 0, width, height );

	mEffectManager->OnScreenSizeChanged( width, height );
}

#endif
