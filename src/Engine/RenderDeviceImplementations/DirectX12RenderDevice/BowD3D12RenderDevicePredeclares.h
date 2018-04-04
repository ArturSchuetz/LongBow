#pragma once

namespace bow {

	class D3DRenderDevice;
	typedef std::shared_ptr<D3DRenderDevice> D3DRenderDevicePtr;
	typedef std::unordered_map<unsigned int, D3DRenderDevice> D3DRenderDeviceMap;

	class D3DGraphicsWindow;
	typedef std::shared_ptr<D3DGraphicsWindow> D3DGraphicsWindowPtr;
	typedef std::unordered_map<unsigned int, D3DGraphicsWindowPtr> D3DGraphicsWindowMap;

	class D3DRenderContext;
	typedef std::shared_ptr<D3DRenderContext> D3DRenderContextPtr;

	class D3DVertexBuffer;
	typedef std::shared_ptr<D3DVertexBuffer> D3DVertexBufferPtr;

	class D3DShaderProgram;
	typedef std::shared_ptr<D3DShaderProgram> D3DShaderProgramPtr;
	
	class D3DFragmentOutputs;
	typedef std::shared_ptr<D3DFragmentOutputs> D3DFragmentOutputsPtr;

	class D3DIndexBuffer;
	typedef std::shared_ptr<D3DIndexBuffer> D3DIndexBufferPtr;

	class D3DVertexArray;
	typedef std::shared_ptr<D3DVertexArray> D3DVertexArrayPtr;

	typedef std::unordered_map<unsigned int, VertexBufferAttributePtr> D3DVertexBufferAttributeMap;

}

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// The min/max macros conflict with like-named member functions.
// Only use std::min and std::max defined in <algorithm>.
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

// In order to define a function called CreateWindow, the Windows macro needs to
// be undefined.
#if defined(CreateWindow)
#undef CreateWindow
#endif

// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>
using namespace Microsoft::WRL;
