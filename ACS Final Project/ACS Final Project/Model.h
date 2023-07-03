#pragma once
#include "Mesh.h"
#include "Texture.h"

//���峣��buffer�ṹ��
struct ST_GRS_FRAME_MVP_BUFFER
{
	XMFLOAT4X4 m_MVP;//�����Model-view-projection(MVP)����
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
	SIZE_T szMVPBuffer = GRS_UPPER_DIV(sizeof(ST_GRS_FRAME_MVP_BUFFER), 256) * 256;//����ߴ�����Ϊ256�߽�����С

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

	//����������Ϊ�ϴ���
	D3D12_HEAP_PROPERTIES stHeapUpload = { D3D12_HEAP_TYPE_UPLOAD };

	D3D12_RESOURCE_DESC stCBVUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(szMVPBuffer);
	//��������������Դ���� ע�⻺��ߴ�����Ϊ256�߽�����С
	GlobalEngine.pID3DDevice->CreateCommittedResource(
		&stHeapUpload,
		D3D12_HEAP_FLAG_NONE,
		&stCBVUploadDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pICBVUpload));

	// Map ֮��Ͳ���Unmap�� ֱ�Ӹ������ݽ�ȥ ����ÿ֡������map-copy-unmap�˷�ʱ����
	pICBVUpload->Map(0, nullptr, reinterpret_cast<void**>(&pMVPBuffer));

	//����SRV�� (Shader Resource View Heap) ������ͼ�������ͳ�����������
	//��������ͼ��������CBV����������һ������������
	D3D12_DESCRIPTOR_HEAP_DESC stSRVCBVHeapDesc = {};
	stSRVCBVHeapDesc.NumDescriptors = 2;
	stSRVCBVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	stSRVCBVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	GlobalEngine.pID3DDevice->CreateDescriptorHeap(
		&stSRVCBVHeapDesc,
		IID_PPV_ARGS(&pISRVCBVHeap));

	//����SRV������
	D3D12_SHADER_RESOURCE_VIEW_DESC stSRVDesc = {};
	stSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	stSRVDesc.Format = UsedTexture.pITexture->GetDesc().Format;
	stSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	stSRVDesc.Texture2D.MipLevels = 1;

	GlobalEngine.pID3DDevice->CreateShaderResourceView(
		UsedTexture.pITexture.Get(),
		&stSRVDesc,
		pISRVCBVHeap->GetCPUDescriptorHandleForHeapStart());

	//����CBV������
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

	//������������
	ID3D12DescriptorHeap* ppHeaps[] = { pISRVCBVHeap.Get(),GlobalEngine.pISamplerDescriptorHeap.Get() };
	GlobalEngine.pICommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	
	//����SRV
	GlobalEngine.pICommandList->SetGraphicsRootDescriptorTable(
		0,
		pISRVCBVHeap->GetGPUDescriptorHandleForHeapStart());

	CD3DX12_GPU_DESCRIPTOR_HANDLE stGPUCBVHandle(
		pISRVCBVHeap->GetGPUDescriptorHandleForHeapStart(),
		1,
		GlobalEngine.pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	//����CBV
	GlobalEngine.pICommandList->SetGraphicsRootDescriptorTable(
		1,
		stGPUCBVHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE hGPUSamplerHandle(
		GlobalEngine.pISamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart(),
		nCurrentSamplerNO,
		GlobalEngine.nSamplerDescriptorSize);
	//����Sampler
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