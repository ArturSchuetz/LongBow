#pragma once
#include <DirectX12ThinDevice/DirectX12ThinDevice_api.h>
#include <ThinRenderDevice/IBowThinDevice.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

namespace bow
{

class D3D12ThinDevice;

//! Translations between the thin enums and DirectX 12.
/*!
    The counterpart of VulkanThinTypes, and worth reading against it: the same
    ThinResourceState becomes one D3D12_RESOURCE_STATES value here where Vulkan
    needs a layout, an access mask and a stage mask. That difference is exactly
    why the enum is part of the interface rather than something a backend
    works out for itself.
*/
class D3D12ThinTypes
{
  public:
    static DXGI_FORMAT ToDXGIFormat(ThinFormat format);
    static ThinFormat FromDXGIFormat(DXGI_FORMAT format);

    static D3D12_RESOURCE_STATES ToResourceStates(ThinResourceState state);

    static D3D12_PRIMITIVE_TOPOLOGY_TYPE ToTopologyType(ThinPrimitiveTopology topology);
    static D3D12_PRIMITIVE_TOPOLOGY ToTopology(ThinPrimitiveTopology topology);
    static D3D12_COMPARISON_FUNC ToComparisonFunc(ThinCompareOp op);
    static D3D12_CULL_MODE ToCullMode(ThinCullMode mode);
    static D3D12_BLEND ToBlend(ThinBlendFactor factor);
    static D3D12_SHADER_VISIBILITY ToShaderVisibility(ThinShaderStage stages);
    static D3D12_DESCRIPTOR_RANGE_TYPE ToDescriptorRangeType(ThinDescriptorType type);

    //! Human-readable form of an HRESULT, for log messages.
    static const char *ToString(HRESULT result);
};

//! Logs and returns false when a DirectX call failed.
bool D3D12Check(HRESULT result, const char *what);

} // namespace bow
