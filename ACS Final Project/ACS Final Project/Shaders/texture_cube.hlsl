struct PSInput
{
	float4 position : SV_POSITION;
	float4 uv : TEXCOORD;
	float4 normal : NORMAL;
};

cbuffer MVPBuffer : register(b0)
{
	float4x4 m_MVP;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(float4 position : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL)
{
	PSInput result;

	result.position = mul(m_MVP, position);
	result.normal = normal;
	result.uv = uv;

	return result;
}



float4 PSMain(PSInput input) : SV_TARGET
{
    float3 lightColor = { 1.0f, 1.0f, 1.0f };
    float3 lightPosition = { 100.0f, 100.0f, -100.0f };
    float3 viewPosition = { 1.0f, 3.0f, -7.0f };
    float3 worldPosition = { 0.0f, 0.0f, 0.0f };

	float4 color = g_texture.Sample(g_sampler, input.uv);
    //ambient
    float ambientStrength = 0.1;
    float3 ambient = mul(ambientStrength, lightColor);

    // diffuse 
    float3 norm = normalize(input.normal);
    float3 lightDir = normalize(lightPosition - viewPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    float3 diffuse = mul(diff, lightColor);

    // specular
    //float specularStrength = 0.5;
    //float3 viewDir = normalize(viewPosition - worldPosition);
    //float3 H = normalize(lightDir + viewDir);
    //float spec = pow(max(dot(H, norm), 0.0), 32);
    //float3 specular = mul(mul(specularStrength, spec), lightColor);

    color = mul((ambient + diffuse), color.xyz);

    return color;
}