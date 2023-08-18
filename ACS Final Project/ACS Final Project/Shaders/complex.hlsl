#define MAX_LIGHTS 16
#define NUM_DIR_LIGHTS 0
#define NUM_POINT_LIGHTS 2
#define NUM_SPOT_LIGHTS 1

struct Light
{
	float3 strength;    //��Դ��ɫ������ͨ�ã�
	float falloffStart;     //���ƺ;۹�ƵĿ�ʼ˥������
	float3 direction;   //�����;۹�Ƶķ�������
	float falloffEnd;    //���;۹�Ƶ�˥����������
	float3 position;    //���;۹�Ƶ�����
	float spotPower;    //�۹�������е��ݲ���
};

struct Material
{
	float4 diffuseAlbedo; //���ʷ����� kd
	float3 fresnelR0; //RF(0)ֵ�������ʵķ������� ks 
	float roughness; //���ʵĴֲڶ� ns
};

//����˥����d�ǹ�Դ�����嶥��ľ��룬falloffEnd�����˥�����룬falloffStart�ǿ�ʼ˥������
float CalcAttenuation(float d, float falloffEnd, float falloffStart)
{
	//d����ƹ�ľ���
	float att = saturate((falloffEnd - d) / (falloffEnd - falloffStart));
	return att;
}

float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVector)
{
	//����ʯ��˷��̼����������߱�����İٷֱ�
	float3 reflectPercent = R0 + (1.0f - R0) * pow(1 - saturate(dot(normal, lightVector)), 5.0f);
	return reflectPercent;
}

//BlinnPhong����ģ��
float3 BlinnPhong(Material mat, float3 normal, float3 toEye, float3 lightVec, float3 lightStrength)
{
	float m = mat.roughness; //�ֲڶ��������mֵ
	float3 halfVec = normalize(lightVec + toEye); //�������
	float roughnessFactor = (m + 8.0f) / 8.0f * pow(max(dot(normal, halfVec), 0.0f), m);//�ֲڶ�����
	float3 fresnelFactor = SchlickFresnel(mat.fresnelR0, halfVec, lightVec); //���������

	float3 specAlbedo = fresnelFactor * roughnessFactor;//���淴�䷴����=���������*�ֲڶ�����
	specAlbedo = specAlbedo / (specAlbedo + 1); //�����淴�䷴�������ŵ�[0��1]

	float3 diff_Spec = lightStrength * (mat.diffuseAlbedo.rgb + specAlbedo); //������+�߹ⷴ��=�������*�ܵķ�����
	return diff_Spec;//����������+�߹ⷴ��
	//return lightStrength;
}

//******************************************************************

static const float PI = 3.14159265359;

struct ST_CB_PBR_MATERIAL
{
	float3   g_v3Albedo;        // ������
	float    g_fMetallic;           // ������
	float    g_fRoughness;      // �ֲڶ�
	float    g_fAO;                  // �������ڱ�
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
	// ֱ�ӹ���ʱ k = (fRoughness + 1)^2 / 8;
	// IBL ʱ k = ��fRoughness^2)/2;
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
	// ʹ�ò�ֵ���ɵĹ⻬����
	float3 N = normalize(normal);
	// ������
	float3 V = normalize(toEye);

	float3 F0 = float3(0.04f, 0.04f, 0.04f);

	// Gamma������ɫ
	float3 v3Albedo = pow(mat.diffuseAlbedo.rgb, 2.2f);
	//float3 v3Albedo = mat.diffuseAlbedo.rgb;

	F0 = lerp(F0, v3Albedo, mat.fresnelR0);

	// ��������
	float3 Lo = float3(0.0f, 0.0f, 0.0f);

	// �ֲڶ�
	float fRoughness = mat.roughness;


	// ���Դ�����
	// ���������
	float3 L = normalize(lightVec);
	// �м�������������뷨�ߵĽ�ƽ���ߣ�
	float3 H = normalize(V + L);

	float3 radiance = lightStrength;

	// Cook-Torrance����ģ�� BRDF
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

//ƽ�й�Դ
float3 ComputerDirectionalLight(Light light, Material mat, float3 normal, float3 toEye)
{
	float3 lightVec = -light.direction; //�������͹�Դָ�򶥵�������෴
	float3 lightStrength = max(dot(normal, lightVec), 0.0f) * light.strength; //����ⵥλ����ϵķ��ն�

	//ƽ�й��������+�߹ⷴ��
	return BlinnPhong(mat, normal, toEye, lightVec, lightStrength);
}

//���Դ
float3 ComputerPointLight(Light light, Material mat, float3 pos, float3 normal, float3 toEye)
{
	float3 lightVec = light.position - pos; //����ָ����Դ�Ĺ�����
	float distance = length(lightVec);//����͹�Դ�ľ��루����ģ����

	if (distance > light.falloffEnd)
		return 0;   //�������˥������ֱ�ӷ���0��ʡ�ļ�����

	lightVec /= distance;//��һ���ɵ�λ����
	float nDotl = max(dot(lightVec, normal), 0);//�������С��0
	float3 lightStrength = nDotl * light.strength;//����ٵ�λ����ϵķ��նȣ�û����˥����

	//����˥������
	float att = CalcAttenuation(distance, light.falloffEnd, light.falloffStart);
	lightStrength *= att;//˥����ĵ�λ������ն�

	//�������������͸߹ⷴ��
	//return BlinnPhong(mat, normal, toEye, lightVec, lightStrength);
	return PBR(mat, normal, toEye, lightVec, lightStrength);
}

//�۹�ƹ�Դ
float3 ComputerSpotLight(Light light, Material mat, float3 pos, float3 normal, float3 toEye)
{
	float3 lightVec = light.position - pos; //����ָ��۹�ƹ�Դ�Ĺ�����
	float distance = length(lightVec); //����͹�Դ�ľ��루����ģ����

	if (distance > light.falloffEnd)
		return 0; //�������˥������ֱ�ӷ���0��ʡ�ļ�����

	lightVec /= distance; //��һ���ɵ�λ����
	float nDotl = max(dot(lightVec, normal), 0); //�������С��0
	float3 lightStrength = nDotl * light.strength; //����ٵ�λ����ϵķ��նȣ�û����˥����

	//����˥������
	float att = CalcAttenuation(distance, light.falloffEnd, light.falloffStart);
	lightStrength *= att; //˥����ĵ�λ������ն�

	//����۹��˥������
	float spotFactor = pow(max(dot(-lightVec, light.direction), 0), light.spotPower);
	lightStrength *= spotFactor;

	//����۹�Ƶ�������͸߹ⷴ��
	return PBR(mat, normal, toEye, lightVec, lightStrength);
}

float4 ComputerLighting(Light lights[MAX_LIGHTS], Material mat, float3 pos, float3 normal, float3 toEye, float3 shadowFactor)
{
	float3 result = 0.0f;
	int i = 0;

#if(NUM_DIR_LIGHTS > 0)
	for (i = 0; i < NUM_DIR_LIGHTS; i++)
	{
		//���ƽ�й�Ĺ��յ���
		result += ComputerDirectionalLight(lights[i], mat, normal, toEye);
	}
#endif

#if(NUM_POINT_LIGHTS > 0)
	for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i++)
	{
		//������Դ�Ĺ��յ���
		result += ComputerPointLight(lights[i], mat, pos, normal, toEye);
	}
#endif

#if(NUM_SPOT_LIGHTS > 0)
	for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; i++)
	{
		//����۹�ƹ�Դ�Ĺ��յ���
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
	float3 shadowFactor = float3(1, 1, 1); //��ʱʹ��1.0�����Լ������Ӱ��
	//ֱ�ӹ���
	float4 directLight = ComputerLighting(lights, mat, input.worldPosition.xyz, worldNormal, worldView, shadowFactor);
	//��������
	float4 ambientL = ambient * diffuseAlbedo;
	//
	float4 finalCol = ambientL + directLight;
	finalCol.a = diffuseAlbedo.a;


	float fAO = 1.0;
	// ��������
	float3 ambientNew = float3(0.03f, 0.03f, 0.03f) * diffuseAlbedo * fAO;
	float3 colorNew = ambientNew + directLight;

	colorNew = colorNew / (colorNew + float3(1.0f, 1.0f, 1.0f));
	// Gamma ����
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