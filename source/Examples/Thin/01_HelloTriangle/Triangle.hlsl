// One source for both thin backends: dxc emits SPIR-V for Vulkan and DXIL for
// DirectX 12 from this file, which is why the thin tier is authored in HLSL.

struct VertexInput
{
    float3 position : TEXCOORD0;
    float3 color    : TEXCOORD1;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float3 color    : TEXCOORD0;
};

VertexOutput mainVS(VertexInput input)
{
    VertexOutput output;
    output.position = float4(input.position, 1.0);
    output.color = input.color;
    return output;
}

float4 mainPS(VertexOutput input) : SV_TARGET
{
    return float4(input.color, 1.0);
}
