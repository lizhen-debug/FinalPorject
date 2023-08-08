#pragma once
#define MAX_LIGHTS 16
struct stAdvancedLight
{
	XMFLOAT3 strength = { 0.5f, 0.5f, 0.5f };//光源颜色（三光通用）
	float falloffStart = 1.0f;	//点光灯和聚光灯的开始衰减距离
	XMFLOAT3 direction = { 0.0f, -1.0f, 0.0f };//方向光和聚光灯的方向向量
	float falloffEnd = 10.0f;//点光和聚光灯的衰减结束距离
	XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };//点光和聚光灯的坐标
	float spotPower = 64.0f;//聚光灯因子中的参数
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
	
	//主光源
	lights[0].direction = { 0.57735f, -0.57735f, 0.57735f };
	lights[0].strength = { 0.6f, 0.6f, 0.6f };
	//轮廓光源
	lights[1].direction = { -0.57735f, -0.57735f, 0.57735f };
	lights[1].strength = { 0.3f, 0.3f, 0.3f };
	//辅助光源
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