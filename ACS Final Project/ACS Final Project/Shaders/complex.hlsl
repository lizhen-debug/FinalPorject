#define MAX_LIGHTS 16
#define NUM_DIR_LIGHTS 0
#define NUM_POINT_LIGHTS 2
#define NUM_SPOT_LIGHTS 1

struct Light
{
	float3 strength;    //光源颜色（三光通用）
	float falloffStart;     //点光灯和聚光灯的开始衰减距离
	float3 direction;   //方向光和聚光灯的方向向量
	float falloffEnd;    //点光和聚光灯的衰减结束距离
	float3 position;    //点光和聚光灯的坐标
	float spotPower;    //聚光灯因子中的幂参数
};

struct Material
{
	float4 diffuseAlbedo; //材质反照率 kd
	float3 fresnelR0; //RF(0)值，即材质的反射属性 ks 
	float roughness; //材质的粗糙度 ns
};

//光照衰减：d是光源离物体顶点的距离，falloffEnd是最大衰减距离，falloffStart是开始衰减距离
float CalcAttenuation(float d, float falloffEnd, float falloffStart)
{
	//d是离灯光的距离
	float att = saturate((falloffEnd - d) / (falloffEnd - falloffStart));
	return att;
}

float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVector)
{
	//根据石里克方程计算出入射光线被反射的百分比
	float3 reflectPercent = R0 + (1.0f - R0) * pow(1 - saturate(dot(normal, lightVector)), 5.0f);
	return reflectPercent;
}

//BlinnPhong光照模型
float3 BlinnPhong(Material mat, float3 normal, float3 toEye, float3 lightVec, float3 lightStrength)
{
	float m = mat.roughness; //粗糙度因子里的m值
	float3 halfVec = normalize(lightVec + toEye); //半角向量
	float roughnessFactor = (m + 8.0f) / 8.0f * pow(max(dot(normal, halfVec), 0.0f), m);//粗糙度因子
	float3 fresnelFactor = SchlickFresnel(mat.fresnelR0, halfVec, lightVec); //菲尼尔因子

	float3 specAlbedo = fresnelFactor * roughnessFactor;//镜面反射反照率=菲尼尔因子*粗糙度因子
	specAlbedo = specAlbedo / (specAlbedo + 1); //将镜面反射反照率缩放到[0，1]

	float3 diff_Spec = lightStrength * (mat.diffuseAlbedo.rgb + specAlbedo); //漫反射+高光反射=入射光量*总的反照率
	return diff_Spec;//返回漫反射+高光反射
	//return lightStrength;
}

//******************************************************************

static const float PI = 3.14159265359;

struct ST_CB_PBR_MATERIAL
{
	float3   g_v3Albedo;        // 反射率
	float    g_fMetallic;           // 金属度
	float    g_fRoughness;      // 粗糙度
	float    g_fAO;                  // 环境光遮蔽
};

float3 Fresnel_Schlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float Distribution_GGX(float3 N, float3 H, float fRoughness)
{
	float a = fRoughness * fRoughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

float Geometry_Schlick_GGX(float NdotV, float fRoughness)
{
	// 直接光照时 k = (fRoughness + 1)^2 / 8;
	// IBL 时 k = （fRoughness^2)/2;
	float r = (fRoughness + 1.0);
	float k = (r * r) / 8.0;
	//float k = pow((fRoughness + 1), 2) / 8;
	//float k = pow(fRoughness, 2) / 2;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float Geometry_Smith(float3 N, float3 V, float3 L, float fRoughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = Geometry_Schlick_GGX(NdotV, fRoughness);
	float ggx1 = Geometry_Schlick_GGX(NdotL, fRoughness);

	return ggx1 * ggx2;
}

float3 PBR(Material mat, float3 normal, float3 toEye, float3 lightVec, float3 lightStrength)
{
	// 使用插值生成的光滑法线
	float3 N = normalize(normal);
	// 视向量
	float3 V = normalize(toEye);

	float3 F0 = float3(0.04f, 0.04f, 0.04f);

	// Gamma矫正颜色
	float3 v3Albedo = pow(mat.diffuseAlbedo.rgb, 2.2f);
	//float3 v3Albedo = mat.diffuseAlbedo.rgb;

	F0 = lerp(F0, v3Albedo, mat.fresnelR0);

	// 出射辐射度
	float3 Lo = float3(0.0f, 0.0f, 0.0f);

	// 粗糙度
	float fRoughness = mat.roughness;


	// 点光源的情况
	// 入射光向量
	float3 L = normalize(lightVec);
	// 中间向量（入射光与法线的角平分线）
	float3 H = normalize(V + L);

	float3 radiance = lightStrength;

	// Cook-Torrance光照模型 BRDF
	float NDF = Distribution_GGX(N, H, fRoughness);
	float G = Geometry_Smith(N, V, L, fRoughness);
	float3 F = Fresnel_Schlick(max(dot(H, V), 0.0), F0);

	float3 kS = F;
	float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
	kD *= 1.0 - mat.fresnelR0;

	float3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	float3 specular = numerator / max(denominator, 0.001);

	// add to outgoing radiance Lo
	float NdotL = max(dot(N, L), 0.0);
	Lo += (kD * v3Albedo / PI + specular) * radiance * NdotL;
	
	float3 diff_Spec = radiance * (v3Albedo + specular);
	
	return diff_Spec;
}

//******************************************************************

//平行光源
float3 ComputerDirectionalLight(Light light, Material mat, float3 normal, float3 toEye)
{
	float3 lightVec = -light.direction; //光向量和光源指向顶点的向量相反
	float3 lightStrength = max(dot(normal, lightVec), 0.0f) * light.strength; //方向光单位面积上的辐照度

	//平行光的漫反射+高光反射
	return BlinnPhong(mat, normal, toEye, lightVec, lightStrength);
}

//点光源
float3 ComputerPointLight(Light light, Material mat, float3 pos, float3 normal, float3 toEye)
{
	float3 lightVec = light.position - pos; //顶点指向点光源的光向量
	float distance = length(lightVec);//顶点和光源的距离（向量模长）

	if (distance > light.falloffEnd)
		return 0;   //如果大于衰减距离直接返回0，省的计算了

	lightVec /= distance;//归一化成单位向量
	float nDotl = max(dot(lightVec, normal), 0);//点积不能小于0
	float3 lightStrength = nDotl * light.strength;//点光再单位面积上的辐照度（没考虑衰减）

	//调用衰减函数
	float att = CalcAttenuation(distance, light.falloffEnd, light.falloffStart);
	lightStrength *= att;//衰减后的单位面积辐照度

	//计算点光的漫反射和高光反射
	//return BlinnPhong(mat, normal, toEye, lightVec, lightStrength);
	return PBR(mat, normal, toEye, lightVec, lightStrength);
}

//聚光灯光源
float3 ComputerSpotLight(Light light, Material mat, float3 pos, float3 normal, float3 toEye)
{
	float3 lightVec = light.position - pos; //顶点指向聚光灯光源的光向量
	float distance = length(lightVec); //顶点和光源的距离（向量模长）

	if (distance > light.falloffEnd)
		return 0; //如果大于衰减距离直接返回0，省的计算了

	lightVec /= distance; //归一化成单位向量
	float nDotl = max(dot(lightVec, normal), 0); //点积不能小于0
	float3 lightStrength = nDotl * light.strength; //点光再单位面积上的辐照度（没考虑衰减）

	//调用衰减函数
	float att = CalcAttenuation(distance, light.falloffEnd, light.falloffStart);
	lightStrength *= att; //衰减后的单位面积辐照度

	//计算聚光灯衰减因子
	float spotFactor = pow(max(dot(-lightVec, light.direction), 0), light.spotPower);
	lightStrength *= spotFactor;

	//计算聚光灯的漫反射和高光反射
	return PBR(mat, normal, toEye, lightVec, lightStrength);
}

float4 ComputerLighting(Light lights[MAX_LIGHTS], Material mat, float3 pos, float3 normal, float3 toEye, float3 shadowFactor)
{
	float3 result = 0.0f;
	int i = 0;

#if(NUM_DIR_LIGHTS > 0)
	for (i = 0; i < NUM_DIR_LIGHTS; i++)
	{
		//多个平行光的光照叠加
		result += ComputerDirectionalLight(lights[i], mat, normal, toEye);
	}
#endif

#if(NUM_POINT_LIGHTS > 0)
	for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i++)
	{
		//多个点光源的光照叠加
		result += ComputerPointLight(lights[i], mat, pos, normal, toEye);
	}
#endif

#if(NUM_SPOT_LIGHTS > 0)
	for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; i++)
	{
		//多个聚光灯光源的光照叠加
		result += ComputerSpotLight(lights[i], mat, pos, normal, toEye);
	}
#endif

	return float4(result, 0.0f);
}

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

	Light lights[16];
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

	if (texDescriptor3[0] != -1)// if diffuse texture exist 
	{
		input.worldPosition += g_texture.Sample(g_sampler, float3(input.texCoord.xy, texDescriptor3[0]));
	}


	float4 ambient = float4(Ka, 1.0f) * 0.3;

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
	float spec = pow(saturate(dot(normalize(2 * saturate(dot(normal, lightDir)) * normal + lightDir), viewDir)), Ns);
	

	float4 finalColor = ambient + diff * diffuse + spec * specular;


	//if (texDescriptor1[1] != -1 )
	//{
	//	finalColor = g_texture.Sample(g_sampler, float3(input.texCoord.xy, texDescriptor1[1]));
	//}
	//else
	//{
	//	finalColor = float4(1, 0, 0, 1);
	//}


	float roughness;
	if (texDescriptor3[2] != -1)// if diffuse texture exist 
	{
		roughness = (1 - g_texture.Sample(g_sampler, float3(input.texCoord.xy, texDescriptor3[2])).g) * 256.0;
	}
	else
	{
		roughness = Ns;
	}


	float4 diffuseAlbedo = diffuse;

	float3 worldNormal = normal.xyz;
	float3 worldView = viewDir.xyz;


	float3 mat_ks;
	if (texDescriptor1[0] != -1)// if diffuse texture exist 
	{
		mat_ks = g_texture.Sample(g_sampler, float3(input.texCoord.xy, texDescriptor1[0])).xyz;
	}
	else
	{
		mat_ks = Ks.xyz;
	}


	Material mat = { diffuseAlbedo, mat_ks, roughness };
	float3 shadowFactor = float3(1, 1, 1); //暂时使用1.0，不对计算产生影响
	//直接光照
	float4 directLight = ComputerLighting(lights, mat, input.worldPosition.xyz, worldNormal, worldView, shadowFactor);
	//环境光照
	float4 ambientL = ambient * diffuseAlbedo;
	//
	float4 finalCol = ambientL + directLight;
	finalCol.a = diffuseAlbedo.a;


	float fAO = 1.0;
	// 环境光项
	float3 ambientNew = float3(0.03f, 0.03f, 0.03f) * diffuseAlbedo * fAO;
	float3 colorNew = ambientNew + directLight;

	colorNew = colorNew / (colorNew + float3(1.0f, 1.0f, 1.0f));
	// Gamma 矫正
	colorNew = pow(colorNew, 1.0f / 2.2f);
	
	return finalCol;
	


	//if (texDescriptor2[0] != -1)// if diffuse texture exist 
	//{
	//	finalCol = g_texture.Sample(g_sampler, float3(input.texCoord.xy, texDescriptor2[0]));
	//}
	//else
	//{
	//	finalCol = float4(1, 0, 0, 1);
	//}

	//return finalCol;
	//if (lights[2].spotPower == 666)
	//	return finalColor;
	//else
	//	return float4(1, 0, 0, 1);
}