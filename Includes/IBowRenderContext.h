#pragma once
#include "LongBow.h"
#include "BowClearState.h"
#include "BowRenderState.h"
#include "IBowVertexArray.h"
#include "IBowShaderProgram.h"
#include "IBowFramebuffer.h"

#include "IBowTexture2D.h"
#include "IBowTextureSampler.h"

namespace Bow {

	enum class PrimitiveType : char
    {
        Points,
        Lines,
        LineLoop,
        LineStrip,
        Triangles,
        TriangleStrip,
        TriangleFan,
        LinesAdjacency,
        LineStripAdjacency,
        TrianglesAdjacency,
        TriangleStripAdjacency
    };

	enum class WindowType : char
	{
		Default = 0,
		FullScreen = 1
	};

	struct Viewport
	{
		Viewport() : x(0), y(0), width(0), height(0) {}
		Viewport(int x, int y, int width, int height) : x(x), y(y), width(width), height(height){}
		int x;
		int y;
		int width;
		int height;

		bool operator == (const Viewport& vp)
		{
			return (x == vp.x && x == vp.y && x == vp.width && x == vp.height);
		}

		bool operator != (const Viewport& vp)
		{
			return !(x == vp.x && x == vp.y && x == vp.width && x == vp.height);
		}
	};

	class IRenderContext
	{
	public:
		virtual	~IRenderContext(void){}

		// =========================================================================
		// INIT/RELEASE STUFF:
		// =========================================================================
		virtual void VRelease(void) = 0;

		// =========================================================================
		// RENDERING STUFF:
		// =========================================================================
		virtual void VMakeCurrent() = 0;
        virtual VertexArrayPtr CreateVertexArray() = 0;
		virtual FramebufferPtr CreateFramebuffer() = 0;

		virtual void VClear(ClearState clearState) = 0;
		virtual void VDraw(PrimitiveType primitiveType, VertexArrayPtr vertexArray, ShaderProgramPtr shaderProgram, RenderState renderState) = 0;
		virtual void VDraw(PrimitiveType primitiveType, int offset, int count, VertexArrayPtr vertexArray, ShaderProgramPtr shaderProgram, RenderState renderState) = 0;

		virtual void VSetTexture(int location, Texture2DPtr texture) = 0;
		virtual void VSetTextureSampler(int location, TextureSamplerPtr sampler) = 0;

		virtual void VSetFramebuffer(FramebufferPtr framebufer) = 0;
		virtual void VSetViewport(Viewport viewport) = 0;
		virtual Viewport VGetViewport(void) = 0;

		virtual void VSwapBuffers(void) = 0;
	};

	typedef std::shared_ptr<IRenderContext> RenderContextPtr;

}