#pragma once
#include "Mesh.h"
#include "Texture.h"

//定义常量buffer结构体
struct ST_GRS_FRAME_MVP_BUFFER
{
	XMFLOAT4X4 m_MVP;//经典的Model-view-projection(MVP)矩阵
};

class Model
{
public:
	Model();
	Model(Engine engine);
	void InitModel(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, Mesh used_mesh, Texture used_texture);
	void RenderModel(XMMATRIX MVP_matrix);
	void RotationY(float rotation_angle);
	~Model();

	XMMATRIX ModelMatrix = {};
	XMFLOAT3 ModelPosition = {};
	XMFLOAT3 ModelRotation = {};
	XMFLOAT3 ModelScale = {};
	

private:
	Engine GlobalEngine;
	Mesh UsedMesh;
	Texture UsedTexture;

	ComPtr<ID3D12DescriptorHeap>		 pISRVCBVHeap;
	ComPtr<ID3D12Resource>			     pICBVUpload;

	ST_GRS_FRAME_MVP_BUFFER* pMVPBuffer = nullptr;
	SIZE_T szMVPBuffer = GRS_UPPER_DIV(sizeof(ST_GRS_FRAME_MVP_BUFFER), 256) * 256;//缓冲尺寸设置为256边界对齐大小

};

Model::Model()
{
}

Model::Model(Engine engine)
{
	GlobalEngine = engine;
}

inline void Model::InitModel(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, Mesh used_mesh, Texture used_texture)
{
	ModelPosition = position;
	ModelRotation = rotation;
	ModelScale = scale;

	ModelMatrix = XMMatrixTranslation(ModelPosition.x, ModelPosition.y, ModelPosition.z);
	ModelMatrix = XMMatrixMultiply(ModelMatrix, XMMatrixRotationRollPitchYaw(ModelRotation.x, ModelRotation.y, ModelRotation.z));
	ModelMatrix = XMMatrixMultiply(ModelMatrix, XMMatrixScaling(ModelScale.x, ModelScale.y, ModelScale.z));

	UsedMesh = used_mesh;
	UsedTexture = used_texture;

	//堆属性设置为上传堆
	D3D12_HEAP_PROPERTIES stHeapUpload = { D3D12_HEAP_TYPE_UPLOAD };

	D3D12_RESOURCE_DESC stCBVUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(szMVPBuffer);
	//创建常量缓冲资源对象 注意缓冲尺寸设置为256边界对齐大小
	GlobalEngine.pID3DDevice->CreateCommittedResource(
		&stHeapUpload,
		D3D12_HEAP_FLAG_NONE,
		&stCBVUploadDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pICBVUpload));

	// Map 之后就不再Unmap了 直接复制数据进去 这样每帧都不用map-copy-unmap浪费时间了
	pICBVUpload->Map(0, nullptr, reinterpret_cast<void**>(&pMVPBuffer));

	//创建SRV堆 (Shader Resource View Heap) 纹理视图描述符和常量描述符堆
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
	cbvDesc.SizeInBytes = static_cast<UINT>(szMVPBuffer);

	CD3DX12_CPU_DESCRIPTOR_HANDLE cbvSrvHandle(
		pISRVCBVHeap->GetCPUDescriptorHandleForHeapStart(),
		1,
		GlobalEngine.pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	GlobalEngine.pID3DDevice->CreateConstantBufferView(&cbvDesc, cbvSrvHandle);

}

inline void Model::RenderModel(XMMATRIX MVP_matrix)
{
	XMStoreFloat4x4(&pMVPBuffer->m_MVP, MVP_matrix);

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

    GlobalEngine.pICommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    GlobalEngine.pICommandList->IASetVertexBuffers(0, 1, &UsedMesh.stVertexBufferView);
	GlobalEngine.pICommandList->IASetIndexBuffer(&UsedMesh.stIndexBufferView);
    GlobalEngine.pICommandList->DrawIndexedInstanced(UsedMesh.nIndicesNum, 1, 0, 0, 0);
	
}

inline void Model::RotationY(float rotation_angle)
{
	
	ModelMatrix = XMMatrixMultiply(ModelMatrix, XMMatrixRotationY(rotation_angle));
	
}

Model::~Model()
{
}