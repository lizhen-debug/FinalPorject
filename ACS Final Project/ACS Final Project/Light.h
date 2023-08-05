#pragma once
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
}

inline void Light::UpdateLight()
{
}

Light::~Light()
{
}