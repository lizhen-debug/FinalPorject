#pragma once
#define MAX_LIGHTS 16
struct stAdvancedLight
{
	XMFLOAT3 strength = { 0.5f, 0.5f, 0.5f };//��Դ��ɫ������ͨ�ã�
	float falloffStart = 1.0f;	//���ƺ;۹�ƵĿ�ʼ˥������
	XMFLOAT3 direction = { 0.0f, -1.0f, 0.0f };//�����;۹�Ƶķ�������
	float falloffEnd = 10.0f;//���;۹�Ƶ�˥����������
	XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };//���;۹�Ƶ�����
	float spotPower = 64.0f;//�۹�������еĲ���
};

class Light
{
public:
	Light();
	void InitLight();
	void UpdateLight();
	~Light();

	XMVECTOR lightColor = {};
	XMVECTOR lightPosition = {};
	XMVECTOR lightDirection = {};

	stAdvancedLight lights[MAX_LIGHTS];
	UINT lights_num;

private:

};

Light::Light()
{
}

inline void Light::InitLight()
{
	lightColor = { 1, 1, 1 };
	lightPosition = { 100, 100, -100 };
	lightDirection = { 0,0.3,-1 };

	lights_num = 3;
	
	//����Դ
	lights[0].direction = { 0.57735f, -0.57735f, 0.57735f };
	lights[0].strength = { 0.6f, 0.6f, 0.6f };
	//������Դ
	lights[1].direction = { -0.57735f, -0.57735f, 0.57735f };
	lights[1].strength = { 0.3f, 0.3f, 0.3f };
	//������Դ
	lights[2].direction = { 0.0f, -0.707f, -0.707f };
	lights[2].strength = { 0.15f, 0.15f, 0.15f };


	lights[2].spotPower = 666;

}

inline void Light::UpdateLight()
{
	//XMMATRIX rotationMatrix = XMMatrixRotationY(0.005);

	//lightDirection = XMVector3TransformNormal(lightDirection, rotationMatrix);
}

Light::~Light()
{
}