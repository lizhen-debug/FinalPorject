struct VSInput {
	float4 position : POSITION;
	float4 normal : NORMAL;
	float4 texCoord : TEXCOORD0;
	float4 tangent : TANGENT;
};

struct PSInput
{
	float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
	float4 texCoord  : TEXCOORD;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
};

cbuffer ConstBuffer : register(b0)
{
	float4x4 m_MVP;
	float4x4 m_RotationMatrix;
	float4x4 m_ModelMatrix;

	float4 f_LightColor;
	float4 f_LightPosition;
    float4 f_LightDirection;
    float4 f_ViewPosition;

	
	float3 Ka;
	float d;
	float3 Kd;
	float Ns;
	float3 Ks;
	int illum;
	float3 Ke;
	float Ni;
	
	float4 texDescriptor1;
	float4 texDescriptor2;
	float4 texDescriptor3;

};

//Texture2D g_texture : register(t0);
Texture2DArray g_texture : register(t0);
SamplerState g_sampler : register(s0);


PSInput VSMain(VSInput input)
{
	PSInput result;
    result.worldPosition = mul(m_ModelMatrix, input.position);
	result.position = mul(m_MVP, input.position);
	result.normal = normalize(mul(m_RotationMatrix, normalize(input.normal)));
	result.texCoord = input.texCoord;
	result.tangent = normalize(mul(m_RotationMatrix, normalize(input.tangent)));

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	float4 ambient = float4(Ka, 1.0f) * 0.2;

	float4 diffuse;
	if (texDescriptor1[0] != -1)// if diffuse texture exist 
	{
		diffuse = g_texture.Sample(g_sampler, float3(input.texCoord.xy, texDescriptor1[0])) * float4(Kd, 1.0f);
	}
	else
	{
		diffuse = float4(Kd, 1.0f);
	}
	
	float4 specular = float4(Ks, 1.0f);

	float4 normalFromMap;
	float4 normal;

	if (texDescriptor1[1] != -1)// if normal texture exist 
	{
		normalFromMap = g_texture.Sample(g_sampler, float3(input.texCoord.xy, texDescriptor1[1])) * 2 - float4(1, 1, 1, 1);
		normal.xyz = normalize(normalFromMap.x * input.tangent + normalFromMap.y * cross(input.normal, input.tangent) + normalFromMap.z * input.normal);
	}
	else
	{
		normal = input.normal;
	}

	
	float3 lightDir = normalize(f_LightDirection.xyz);

	float diff = max(0.0f, dot(normal, lightDir));
	
	
	
	float4 viewDir = normalize(f_ViewPosition - input.worldPosition);
	float spec = pow(saturate(dot(normalize(2 * saturate(dot(normal, -lightDir)) * normal + lightDir), viewDir)), Ns);
	

	float4 finalColor = ambient + diff * diffuse + spec * specular;


	//if (texDescriptor1[1] != -1 )
	//{
	//	finalColor = g_texture.Sample(g_sampler, float3(input.texCoord.xy, texDescriptor1[1]));
	//}
	//else
	//{
	//	finalColor = float4(1, 0, 0, 1);
	//}

	return finalColor;
	
}