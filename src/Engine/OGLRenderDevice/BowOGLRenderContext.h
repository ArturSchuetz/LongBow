#pragma once
#include "BowPrerequisites.h"

#include "IBowRenderContext.h"
#include "BowRenderState.h"

#include <GL/glew.h>
#include <Windows.h>

namespace Bow {
	namespace Renderer {

		typedef std::shared_ptr<class OGLShaderProgram> OGLShaderProgramPtr;
		typedef std::shared_ptr<class OGLTextureUnits> OGLTextureUnitsPtr;
		typedef std::shared_ptr<class OGLFramebuffer> OGLFramebufferPtr;

		enum class StencilFace : GLenum
		{
			Front = GL_FRONT,
			Back = GL_BACK,
			FrontAndBack = GL_FRONT_AND_BACK
		};

		enum class MaterialFace : GLenum
		{
			Front = GL_FRONT,
			Back = GL_BACK,
			FrontAndBack = GL_FRONT_AND_BACK
		};

		class OGLRenderContext : public IRenderContext
		{
		public:
			OGLRenderContext(void);
			~OGLRenderContext(void);

			// =========================================================================
			// INIT/RELEASE STUFF:
			// =========================================================================
			bool	VInitialize(HWND hWnd, int width, int height);
			void	VRelease(void);

			// =========================================================================
			// RENDERING STUFF:
			// =========================================================================
			void	VMakeCurrent(void);
			VertexArrayPtr	CreateVertexArray();
			FramebufferPtr	CreateFramebuffer();

			void	VClear(struct ClearState clearState);
			void	VDraw(PrimitiveType primitiveType, VertexArrayPtr vertexArray, ShaderProgramPtr shaderProgram, RenderState renderState);
			void	VDraw(PrimitiveType primitiveType, int offset, int count, VertexArrayPtr vertexArray, ShaderProgramPtr shaderProgram, RenderState renderState);
			void	Draw(PrimitiveType primitiveType, int offset, int count, VertexArrayPtr vertexArray, ShaderProgramPtr shaderProgram, RenderState renderState);

			void	VSetTexture(int location, Texture2DPtr texture);
			void	VSetTextureSampler(int location, TextureSamplerPtr sampler);
			void	VSetFramebuffer(FramebufferPtr framebufer);
			void	VSetViewport(Viewport viewport);
			Viewport VGetViewport(void);

			void	VSwapBuffers(void);

		private:
			void ApplyRenderState(RenderState renderState);
			void ApplyVertexArray(VertexArrayPtr vertexArray);
			void ApplyShaderProgram(ShaderProgramPtr shaderProgram);

			void ForceApplyRenderState(RenderState renderState);
			void ForceApplyRenderStateStencil(StencilFace face, StencilTestFace test);
			void ApplyPrimitiveRestart(PrimitiveRestart primitiveRestart);
			void ApplyFaceCulling(FaceCulling FaceCulling);
			void ApplyProgramPointSize(ProgramPointSize programPointSize);
			void ApplyRasterizationMode(RasterizationMode rasterizationMode);
			void ApplyScissorTest(ScissorTest scissorTest);
			void ApplyStencilTest(StencilTest stencilTest);
			void ApplyStencil(StencilFace face, StencilTestFace currentTest, StencilTestFace test);
			void ApplyDepthTest(DepthTest depthTest);
			void ApplyDepthRange(DepthRange depthRange);
			void ApplyBlending(Blending blending);
			void ApplyColorMask(ColorMask colorMask);
			void ApplyDepthMask(bool depthMask);

			void ApplyFramebuffer();

			int		m_width;
			int		m_height;

			HGLRC	m_hrc;
			HDC		m_hdc;

			Viewport m_viewport;

			float	m_clearColor[4];
			float	m_clearDepth;
			int		m_clearStencil;

			RenderState			m_renderState;
			OGLShaderProgramPtr m_boundShaderProgram;
			OGLTextureUnitsPtr	m_textureUnits;

			OGLFramebufferPtr	m_boundFramebuffer;
			OGLFramebufferPtr	m_setFramebuffer;

			HWND	m_hWnd;
		};

		typedef std::shared_ptr<OGLRenderContext> OGLRenderContextPtr;

	}
}