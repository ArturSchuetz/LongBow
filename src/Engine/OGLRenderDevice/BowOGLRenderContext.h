#pragma once
#include "BowPrerequisites.h"
#include "BowCorePredeclares.h"
#include "BowRendererPredeclares.h"

#include "IBowRenderContext.h"
#include "BowRenderState.h"

#include "BowMath.h"

struct GLFWwindow;

namespace Bow {
	namespace Renderer {

		typedef std::shared_ptr<class OGLShaderProgram> OGLShaderProgramPtr;
		typedef std::shared_ptr<class OGLTextureUnits> OGLTextureUnitsPtr;
		typedef std::shared_ptr<class OGLFramebuffer> OGLFramebufferPtr;

		enum class StencilFace : unsigned int;
		enum class MaterialFace : unsigned int;

		class OGLRenderContext : public IRenderContext
		{
		public:
			OGLRenderContext(GLFWwindow* window);
			~OGLRenderContext(void);

			// =========================================================================
			// INIT/RELEASE STUFF:
			// =========================================================================
			bool	Initialize();
			void	VRelease(void);

			// =========================================================================
			// RENDERING STUFF:
			// =========================================================================
			VertexArrayPtr	VCreateVertexArray();
			FramebufferPtr	VCreateFramebuffer();

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
			//you shall not copy
			OGLRenderContext(OGLRenderContext&){}
			OGLRenderContext& operator=(const OGLRenderContext&) { return *this; }

			void ApplyVertexArray(VertexArrayPtr vertexArray);
			void ApplyShaderProgram(ShaderProgramPtr shaderProgram);
			void ApplyFramebuffer();

			void ForceApplyRenderState(RenderState renderState);
			void ForceApplyRenderStateStencil(StencilFace face, StencilTestFace test);

			void ApplyRenderState(RenderState renderState);
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

			Viewport			m_viewport;

			Core::ColorRGBA		m_clearColor;
			float				m_clearDepth;
			int					m_clearStencil;

			RenderState			m_renderState;
			OGLShaderProgramPtr	m_boundShaderProgram;
			OGLTextureUnitsPtr	m_textureUnits;

			OGLFramebufferPtr	m_boundFramebuffer;
			OGLFramebufferPtr	m_setFramebuffer;

			GLFWwindow*					m_window;
			static OGLRenderContext*	m_currentContext;
			bool						m_initialized;
		};

		typedef std::shared_ptr<OGLRenderContext> OGLRenderContextPtr;

	}
}