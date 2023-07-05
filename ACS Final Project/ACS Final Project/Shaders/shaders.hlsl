struct PSInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};


cbuffer MVPBuffer : register(b0)
{
	float4x4 m_MVP;
};


PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
{
	PSInput result;

	result.position = mul(m_MVP, position);
	result.color = color;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return input.color;
}