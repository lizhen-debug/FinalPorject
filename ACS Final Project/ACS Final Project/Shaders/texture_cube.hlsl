struct PSInput
{
	float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
	float4 uv : TEXCOORD;
	float4 normal : NORMAL;
};

cbuffer ConstVaribleBuffer : register(b0)
{
	float4x4 m_MVP;
	float4x4 m_RotationMatrix;
	float4x4 m_ModelMatrix;

	float4 f_LightColor;
	float4 f_LightPosition;
    float4 f_LightDirection;

    float4 f_ViewPosition;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);


PSInput VSMain(float4 position : POSITION, float4 uv : TEXCOORD, float4 normal : NORMAL)
{
	PSInput result;
    result.worldPosition = mul(m_ModelMatrix, position);
	result.position = mul(m_MVP, position);
	result.normal = normalize(mul(m_RotationMatrix, normalize(normal)));
	result.uv = uv;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	float4 color = g_texture.Sample(g_sampler, input.uv);
   
	float ambient = 0.5f;
	float f_SpecularPower = 32;

    float4 norm = normalize(input.normal);
    float4 lightDir = normalize(f_LightDirection);
    float diffuse = saturate(dot(norm, lightDir));
	
	float specularStrength = 0.5;
	float4 viewDir = normalize(f_ViewPosition - input.worldPosition);
	float4 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	float4 result = (ambient + diffuse + spec) * color;

    return result;
}