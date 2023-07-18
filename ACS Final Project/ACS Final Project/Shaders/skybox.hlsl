cbuffer MVPBuffer : register(b0)
{
    float4x4 m_MVP;
};

TextureCube	g_txCubeMap : register(t0);
SamplerState g_sampler : register(s0);


struct PSInput
{
    float4 position : SV_POSITION;
    float3 uv  : TEXCOORD0;
};

PSInput VSMain(float4 position : POSITION)
{
    PSInput result;

    result.position = position;
    result.uv = normalize(mul(position, m_MVP).xyz);

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return g_txCubeMap.Sample(g_sampler, input.uv);
}