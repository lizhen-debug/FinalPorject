#define MAX_LIGHTS 16
#define NUM_DIR_LIGHTS 3
#define NUM_POINT_LIGHTS 0
#define NUM_SPOT_LIGHTS 0

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
	float4 diffuseAlbedo; //���ʷ�����
	float3 fresnelR0; //RF(0)ֵ�������ʵķ�������
	float roughness; //���ʵĴֲڶ�
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
	return BlinnPhong(mat, normal, toEye, lightVec, lightStrength);
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
	return BlinnPhong(mat, normal, toEye, lightVec, lightStrength);
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


	float4 ambient = float4(Ka, 1.0f) * 0.5;

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

	Material mat = { diffuseAlbedo, Ks, roughness };
	float3 shadowFactor = float3(1, 1, 1); //��ʱʹ��1.0�����Լ������Ӱ��
	//ֱ�ӹ���
	float4 directLight = ComputerLighting(lights, mat, input.worldPosition.xyz, worldNormal, worldView, shadowFactor);
	//��������
	float4 ambientL = ambient * diffuseAlbedo;
	//
	float4 finalCol = ambientL + directLight;
	finalCol.a = diffuseAlbedo.a;

	return finalCol;
	//if (lights[2].spotPower == 666)
	//	return finalColor;
	//else
	//	return float4(1, 0, 0, 1);
}