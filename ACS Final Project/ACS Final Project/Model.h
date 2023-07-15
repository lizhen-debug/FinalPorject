#pragma once
#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"
#include "Light.h"

//定义常量buffer结构体
struct ConstVaribleBuffer
{
	XMFLOAT4X4 m_MVP;//经典的Model-view-projection(MVP)矩阵
	
	XMFLOAT4X4 m_RotationMatrix;
	XMFLOAT4X4 m_ModelMatrix;
	
	XMFLOAT4 f_LightColor;
	XMFLOAT4 f_LightPosition;
	XMFLOAT4 f_LightDirection;
	XMFLOAT4 f_ViewPosition;

};

enum RenderMethod
{
	IndexedInstanced, Instanced
};

class Model
{
public:
	Model();
	Model(Engine engine);
	void InitModel(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, Mesh used_mesh, Texture used_texture, RenderMethod render_method, D3D_PRIMITIVE_TOPOLOGY toplogy);
	void InitDefaultModel(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, Mesh used_mesh, RenderMethod render_method, D3D_PRIMITIVE_TOPOLOGY toplogy);
	void RenderModel(Camera camera, Light light);
	void RotationY(float rotation_angle);
	~Model();

	XMMATRIX ModelMatrix = {};

	XMMATRIX TranslationMatrix = {};
	XMMATRIX RotationMatrix = {};
	XMMATRIX ScaleMatrix = {};

	XMFLOAT3 ModelPosition = {};
	XMFLOAT3 ModelRotation = {};
	XMFLOAT3 ModelScale = {};
	RenderMethod ModelRenderMethod = IndexedInstanced;
	D3D_PRIMITIVE_TOPOLOGY ModelToplogy = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

private:
	Engine GlobalEngine;
	Mesh UsedMesh;
	Texture UsedTexture;

	ComPtr<ID3D12DescriptorHeap>		 pISRVCBVHeap;
	ComPtr<ID3D12Resource>			     pICBVUpload;

	ConstVaribleBuffer* pConstVarBuffer = nullptr;
	SIZE_T szConstVarBuffer = GRS_UPPER_DIV(sizeof(ConstVaribleBuffer), 256) * 256;//缓冲尺寸设置为256边界对齐大小

};

Model::Model()
{
}

Model::Model(Engine engine)
{
	GlobalEngine = engine;
}

inline void Model::InitModel(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, Mesh used_mesh, Texture used_texture, RenderMethod render_method, D3D_PRIMITIVE_TOPOLOGY toplogy)
{
	ModelPosition = position;
	ModelRotation = rotation;
	ModelScale = scale;

	TranslationMatrix = XMMatrixTranslation(ModelPosition.x, ModelPosition.y, ModelPosition.z);
	RotationMatrix = XMMatrixRotationRollPitchYaw(ModelRotation.x, ModelRotation.y, ModelRotation.z);
	ScaleMatrix = XMMatrixScaling(ModelScale.x, ModelScale.y, ModelScale.z);

	UsedMesh = used_mesh;
	UsedTexture = used_texture;
	ModelRenderMethod = render_method;
	ModelToplogy = toplogy;

	//堆属性设置为上传堆
	D3D12_HEAP_PROPERTIES stHeapUpload = { D3D12_HEAP_TYPE_UPLOAD };

	D3D12_RESOURCE_DESC stCBVUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(szConstVarBuffer);
	//创建常量缓冲资源对象 注意缓冲尺寸设置为256边界对齐大小
	GlobalEngine.pID3DDevice->CreateCommittedResource(
		&stHeapUpload,
		D3D12_HEAP_FLAG_NONE,
		&stCBVUploadDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pICBVUpload));
	
	// Map 之后就不再Unmap了 直接复制数据进去 这样每帧都不用map-copy-unmap浪费时间了
	pICBVUpload->Map(0, nullptr, reinterpret_cast<void**>(&pConstVarBuffer));

	//创建SRV堆 (Shader Resource View Heap) 纹理视图描述符堆,和CBV(Const Buffer View Heap)常量描述符堆
	//将纹理视图描述符和CBV描述符放在一个描述符堆上
	D3D12_DESCRIPTOR_HEAP_DESC stSRVCBVHeapDesc = {};
	stSRVCBVHeapDesc.NumDescriptors = 2;
	stSRVCBVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	stSRVCBVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	GlobalEngine.pID3DDevice->CreateDescriptorHeap(
		&stSRVCBVHeapDesc,
		IID_PPV_ARGS(&pISRVCBVHeap));

	//创建SRV描述符
	D3D12_SHADER_RESOURCE_VIEW_DESC stSRVDesc = {};
	stSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	stSRVDesc.Format = UsedTexture.pITexture->GetDesc().Format;
	stSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	stSRVDesc.Texture2D.MipLevels = 1;

	GlobalEngine.pID3DDevice->CreateShaderResourceView(
		UsedTexture.pITexture.Get(),
		&stSRVDesc,
		pISRVCBVHeap->GetCPUDescriptorHandleForHeapStart());

	//创建CBV描述符
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = pICBVUpload->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(szConstVarBuffer);

	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvSrvHandle(
		pISRVCBVHeap->GetCPUDescriptorHandleForHeapStart(),
		1,
		GlobalEngine.pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	GlobalEngine.pID3DDevice->CreateConstantBufferView(&cbvDesc, cbvSrvHandle);

}

inline void Model::InitDefaultModel(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, Mesh used_mesh, RenderMethod render_method, D3D_PRIMITIVE_TOPOLOGY toplogy)
{
	ModelPosition = position;
	ModelRotation = rotation;
	ModelScale = scale;

	ModelMatrix = XMMatrixTranslation(ModelPosition.x, ModelPosition.y, ModelPosition.z);
	ModelMatrix = XMMatrixMultiply(ModelMatrix, XMMatrixRotationRollPitchYaw(ModelRotation.x, ModelRotation.y, ModelRotation.z));
	ModelMatrix = XMMatrixMultiply(ModelMatrix, XMMatrixScaling(ModelScale.x, ModelScale.y, ModelScale.z));

	UsedMesh = used_mesh;
	ModelRenderMethod = render_method;
	ModelToplogy = toplogy;

	//堆属性设置为上传堆
	D3D12_HEAP_PROPERTIES stHeapUpload = { D3D12_HEAP_TYPE_UPLOAD };

	D3D12_RESOURCE_DESC stCBVUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(szConstVarBuffer);
	//创建常量缓冲资源对象 注意缓冲尺寸设置为256边界对齐大小
	GlobalEngine.pID3DDevice->CreateCommittedResource(
		&stHeapUpload,
		D3D12_HEAP_FLAG_NONE,
		&stCBVUploadDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pICBVUpload));

	// Map 之后就不再Unmap了 直接复制数据进去 这样每帧都不用map-copy-unmap浪费时间了
	pICBVUpload->Map(0, nullptr, reinterpret_cast<void**>(&pConstVarBuffer));

	//创建SRV堆 (Shader Resource View Heap) 纹理视图描述符堆,和CBV(Const Buffer View Heap)常量描述符堆
	//将纹理视图描述符和CBV描述符放在一个描述符堆上
	D3D12_DESCRIPTOR_HEAP_DESC stSRVCBVHeapDesc = {};
	stSRVCBVHeapDesc.NumDescriptors = 2;
	stSRVCBVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	stSRVCBVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	GlobalEngine.pID3DDevice->CreateDescriptorHeap(
		&stSRVCBVHeapDesc,
		IID_PPV_ARGS(&pISRVCBVHeap));

	//创建CBV描述符
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = pICBVUpload->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(szConstVarBuffer);

	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvSrvHandle(
		pISRVCBVHeap->GetCPUDescriptorHandleForHeapStart(),
		1,
		GlobalEngine.pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	GlobalEngine.pID3DDevice->CreateConstantBufferView(&cbvDesc, cbvSrvHandle);
}

inline void Model::RenderModel(Camera camera, Light light)
{

	TranslationMatrix = XMMatrixTranslation(ModelPosition.x, ModelPosition.y, ModelPosition.z);
	RotationMatrix = XMMatrixRotationRollPitchYaw(ModelRotation.x, ModelRotation.y, ModelRotation.z);
	ScaleMatrix = XMMatrixScaling(ModelScale.x, ModelScale.y, ModelScale.z);

	ModelMatrix = XMMatrixMultiply(TranslationMatrix, RotationMatrix);
	ModelMatrix = XMMatrixMultiply(ModelMatrix, ScaleMatrix);

	//计算 模型矩阵 model * 视矩阵 view
	XMMATRIX xmMVP = XMMatrixMultiply(ModelMatrix, camera.ViewMatrix);
	//最后与投影矩阵 projection 相乘
	xmMVP = XMMatrixMultiply(xmMVP, camera.ProjectionMatrix);

	XMStoreFloat4x4(&pConstVarBuffer->m_MVP, xmMVP);
	XMStoreFloat4x4(&pConstVarBuffer->m_RotationMatrix, RotationMatrix);
	XMStoreFloat4x4(&pConstVarBuffer->m_ModelMatrix, ModelMatrix);

	XMStoreFloat4(&pConstVarBuffer->f_LightColor, light.lightColor);
	XMStoreFloat4(&pConstVarBuffer->f_LightPosition, light.lightPosition);
	XMStoreFloat4(&pConstVarBuffer->f_LightDirection, light.lightDirection);
	XMStoreFloat4(&pConstVarBuffer->f_ViewPosition, camera.CameraPosition);
	
	//设置描述符堆
	ID3D12DescriptorHeap* ppHeaps[] = { pISRVCBVHeap.Get(),GlobalEngine.pISamplerDescriptorHeap.Get() };
	GlobalEngine.pICommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	
	//设置SRV
	GlobalEngine.pICommandList->SetGraphicsRootDescriptorTable(
		0,
		pISRVCBVHeap->GetGPUDescriptorHandleForHeapStart());

	CD3DX12_GPU_DESCRIPTOR_HANDLE stGPUCBVHandle(
		pISRVCBVHeap->GetGPUDescriptorHandleForHeapStart(),
		1,
		GlobalEngine.pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	//设置CBV
	GlobalEngine.pICommandList->SetGraphicsRootDescriptorTable(
		1,
		stGPUCBVHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE hGPUSamplerHandle(
		GlobalEngine.pISamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart(),
		nCurrentSamplerNO,
		GlobalEngine.nSamplerDescriptorSize);
	//设置Sampler
	GlobalEngine.pICommandList->SetGraphicsRootDescriptorTable(
		2,
		hGPUSamplerHandle);
	
    GlobalEngine.pICommandList->IASetPrimitiveTopology(ModelToplogy);
    GlobalEngine.pICommandList->IASetVertexBuffers(0, 1, &UsedMesh.stVertexBufferView);
	GlobalEngine.pICommandList->IASetIndexBuffer(&UsedMesh.stIndexBufferView);

	switch (ModelRenderMethod)
	{
	case IndexedInstanced:
		GlobalEngine.pICommandList->SetPipelineState(GlobalEngine.pIPipelineState1.Get());
		GlobalEngine.pICommandList->DrawIndexedInstanced(UsedMesh.nIndicesNum, 1, 0, 0, 0);
		break;
	case Instanced:
		GlobalEngine.pICommandList->SetPipelineState(GlobalEngine.pIPipelineState2.Get());
		GlobalEngine.pICommandList->DrawInstanced(UsedMesh.nVerticesNum, 1, 0, 0);
		break;
	default:
		break;
	}
}

inline void Model::RotationY(float rotation_angle)
{
	ModelRotation.y += rotation_angle;
}

Model::~Model()
{
}