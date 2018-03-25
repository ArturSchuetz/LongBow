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
