#pragma once
#include "Mesh.h"
#include "Texture.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"

struct ConstBuffer
{
	XMFLOAT4X4 m_MVP;
	XMFLOAT4X4 m_RotationMatrix;
	XMFLOAT4X4 m_ModelMatrix;

	XMFLOAT4 f_LightColor;
	XMFLOAT4 f_LightPosition;
	XMFLOAT4 f_LightDirection;
	XMFLOAT4 f_ViewPosition;

	
	XMFLOAT3 Ka;
	float d;
	XMFLOAT3 Kd;
	float Ns;
	XMFLOAT3 Ks;
	int illum;
	XMFLOAT3 Ke;

	float Ni;
	
	

};

struct CModelMeshVertex
{
	XMFLOAT3 Position;//顶点位置信息，float[3]
	XMFLOAT2 TextureCoordinate;//顶点纹理坐标信息，float[2]
	XMFLOAT3 Normal;//顶点法线信息，float[3]
};

class ComplexModel
{
public:
	ComplexModel();
	ComplexModel(Engine engine);
	void InitComplexModel(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, const char* ModelFilePath);
	void RenderModel(Camera camera, Light light);
	~ComplexModel();

	XMMATRIX ModelMatrix = {};

	XMMATRIX TranslationMatrix = {};
	XMMATRIX RotationMatrix = {};
	XMMATRIX ScaleMatrix = {};

	XMFLOAT3 ModelPosition = {};
	XMFLOAT3 ModelRotation = {};
	XMFLOAT3 ModelScale = {};
	
	

private:
	Engine GlobalEngine;

	int NumMeshes = 0;

	//一个用于提供给绘制命令队列的顶点buffer的view，描述了顶点buffer的信息
	D3D12_VERTEX_BUFFER_VIEW* stVertexBufferView = {};
	//一个用于提供给绘制命令队列的顶点buffer的view，描述了索引buffer的信息
	D3D12_INDEX_BUFFER_VIEW* stIndexBufferView = {};
	UINT* nIndicesNum = {};
	UINT* nVerticesNum = {};
	D3D_PRIMITIVE_TOPOLOGY* ModelToplogy = {};
	void LoadModelMtl(const char* ModelMtlPath, map<string, Material> *mtl_map);

	ComPtr<ID3D12DescriptorHeap>*		 pISRVCBVHeap;
	ComPtr<ID3D12Resource>*			     pICBVUpload;
	ComPtr<ID3D12Resource>*				 pIVertexBuffer;
	ComPtr<ID3D12Resource>*				 pIIndexBuffer;

	ConstBuffer** pConstBuffer = {};
	SIZE_T szConstBuffer = GRS_UPPER_DIV(sizeof(ConstBuffer), 256) * 256;

	Texture texture;
	Material* mesh_mtl = {};
};

ComplexModel::ComplexModel()
{
}

ComplexModel::ComplexModel(Engine engine)
{
	GlobalEngine = engine;
}

inline void ComplexModel::InitComplexModel(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, const char* ModelFilePath)
{
	assert(ModelFilePath != nullptr);
	const std::string filePath(ModelFilePath);
	Assimp::Importer importer;
	const std::uint32_t flags{ aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded };
	const aiScene* scene{ importer.ReadFile(filePath.c_str(), flags) };
	assert(scene != nullptr);
	assert(scene->HasMeshes());
	NumMeshes = scene->mNumMeshes;

	map<string, Material> mtl_map = {};
	const char* mtl_path = "D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Models\\Scene\\untitled.mtl";
	LoadModelMtl(mtl_path, &mtl_map);
	
	stVertexBufferView = new D3D12_VERTEX_BUFFER_VIEW[NumMeshes];
	stIndexBufferView = new D3D12_INDEX_BUFFER_VIEW[NumMeshes];
	nVerticesNum = new UINT[NumMeshes];
	nIndicesNum = new UINT[NumMeshes];
	pIVertexBuffer = new ComPtr<ID3D12Resource>[NumMeshes];
	pIIndexBuffer = new ComPtr<ID3D12Resource>[NumMeshes];
	pICBVUpload = new ComPtr<ID3D12Resource>[NumMeshes];
	pISRVCBVHeap = new ComPtr<ID3D12DescriptorHeap>[NumMeshes];
	ModelToplogy = new D3D_PRIMITIVE_TOPOLOGY[NumMeshes];
	mesh_mtl = new Material[NumMeshes];
	pConstBuffer = new ConstBuffer * [NumMeshes];

	//堆属性设置为上传堆
	D3D12_HEAP_PROPERTIES stHeapUpload = { D3D12_HEAP_TYPE_UPLOAD };//Upload的概念就是从CPU内存上传到显存中的意思

	for (std::uint32_t i = 0; i < NumMeshes; ++i)
	{
		aiMesh* mesh{ scene->mMeshes[i] };
		assert(mesh != nullptr);

		vector<CModelMeshVertex> mesh_vertex = {};
		mesh_vertex.clear();
		vector<int> mesh_index = {};
		mesh_index.clear();

		int mesh_index_num = i;
		mesh_mtl[i] = mtl_map[scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str()];

		{
			// Positions
			const std::size_t numVertices{ mesh->mNumVertices };
			assert(numVertices > 0U);
			mesh_vertex.resize(numVertices);
			for (std::uint32_t i = 0U; i < numVertices; ++i)
			{
				mesh_vertex[i].Position = XMFLOAT3(reinterpret_cast<const float*>(&mesh->mVertices[i]));
				if (mesh->HasTextureCoords(0))
				{
					mesh_vertex[i].TextureCoordinate = XMFLOAT2(reinterpret_cast<const float*>(&mesh->mTextureCoords[0][i]));
				}
				if (mesh->HasNormals())
				{
					mesh_vertex[i].Normal = XMFLOAT3(reinterpret_cast<const float*>(&mesh->mNormals[i]));
				}
			}

			// Indices
			const std::uint32_t numFaces{ mesh->mNumFaces };
			assert(numFaces > 0U);
			for (std::uint32_t i = 0U; i < numFaces; ++i)
			{
				const aiFace* face = &mesh->mFaces[i];
				assert(face != nullptr);

				//assert(face->mNumIndices == 3U);
				if (face->mNumIndices == 3U)
				{
					ModelToplogy[mesh_index_num] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
					mesh_index.push_back(face->mIndices[0U]);
					mesh_index.push_back(face->mIndices[1U]);
					mesh_index.push_back(face->mIndices[2U]);
				}
				else if (face->mNumIndices == 2U)
				{
					ModelToplogy[mesh_index_num] = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
					mesh_index.push_back(face->mIndices[0U]);
					mesh_index.push_back(face->mIndices[1U]);
				}

			}
		}

		const UINT nVertexBufferSize = sizeof(mesh_vertex) * mesh_vertex.size();

		const UINT nIndexBufferSize = sizeof(int) * mesh_index.size();

		nIndicesNum[i] = mesh_index.size();
		nVerticesNum[i] = mesh_vertex.size();

		//创建顶点缓冲
		D3D12_RESOURCE_DESC stResDescVertex = CD3DX12_RESOURCE_DESC::Buffer(nVertexBufferSize);
		GlobalEngine.pID3DDevice->CreateCommittedResource(
			&stHeapUpload,
			D3D12_HEAP_FLAG_NONE,
			&stResDescVertex,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&pIVertexBuffer[i]));
		//上述核心即为得到要绘制的顶点buffer，用于下面将绘制数据从CPU内存向显存进行传递
		UINT8* pVertexDataBegin = nullptr;
		CD3DX12_RANGE readRange(0, 0);
		pIVertexBuffer[i]->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		memcpy(pVertexDataBegin, &mesh_vertex[0], nVertexBufferSize);
		pIVertexBuffer[i]->Unmap(0, nullptr);

		//填充顶点buffer视图的结构体，告诉GPU被描述的资源实际是Vertex Buffer
		stVertexBufferView[i].BufferLocation = pIVertexBuffer[i]->GetGPUVirtualAddress();
		stVertexBufferView[i].StrideInBytes = sizeof(ModelMeshVertex);
		stVertexBufferView[i].SizeInBytes = nVertexBufferSize;


		//创建索引缓冲
		D3D12_RESOURCE_DESC stResDescIndex = CD3DX12_RESOURCE_DESC::Buffer(nIndexBufferSize);
		GlobalEngine.pID3DDevice->CreateCommittedResource(
			&stHeapUpload,
			D3D12_HEAP_FLAG_NONE,
			&stResDescIndex,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&pIIndexBuffer[i]));

		//上述核心即为得到要绘制的顶点索引buffer，用于下面将绘制数据从CPU内存向显存进行传递
		UINT8* pIndexDataBegin = nullptr;
		pIIndexBuffer[i]->Map(0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin));
		memcpy(pIndexDataBegin, &mesh_index[0], nIndexBufferSize);
		pIIndexBuffer[i]->Unmap(0, nullptr);

		//填充顶点buffer视图的结构体，告诉GPU被描述的资源是Index Buffer
		stIndexBufferView[i].BufferLocation = pIIndexBuffer[i]->GetGPUVirtualAddress();
		stIndexBufferView[i].Format = DXGI_FORMAT_R32_UINT;
		stIndexBufferView[i].SizeInBytes = nIndexBufferSize;




		D3D12_RESOURCE_DESC stCBVUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(szConstBuffer);
		//创建常量缓冲资源对象 注意缓冲尺寸设置为256边界对齐大小
		GlobalEngine.pID3DDevice->CreateCommittedResource(
			&stHeapUpload,
			D3D12_HEAP_FLAG_NONE,
			&stCBVUploadDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&pICBVUpload[i]));

		// Map 之后就不再Unmap了 直接复制数据进去 这样每帧都不用map-copy-unmap浪费时间了
		pICBVUpload[i]->Map(0, nullptr, reinterpret_cast<void**>(&pConstBuffer[i]));


		TCHAR TextureFileName[] = _T("D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Models\\Scene\\textures\\Metal03_baseColor.jpeg");
		texture = Texture(GlobalEngine);
		texture.LoadTexture(TextureFileName, D3D12_SRV_DIMENSION_TEXTURE2D);


		//创建SRV堆 (Shader Resource View Heap) 纹理视图描述符堆,和CBV(Const Buffer View Heap)常量描述符堆
		//将纹理视图描述符和CBV描述符放在一个描述符堆上
		D3D12_DESCRIPTOR_HEAP_DESC stSRVCBVHeapDesc = {};
		stSRVCBVHeapDesc.NumDescriptors = 2;
		stSRVCBVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		stSRVCBVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		GlobalEngine.pID3DDevice->CreateDescriptorHeap(
			&stSRVCBVHeapDesc,
			IID_PPV_ARGS(&pISRVCBVHeap[i]));

		//创建SRV描述符
		D3D12_SHADER_RESOURCE_VIEW_DESC stSRVDesc = {};
		stSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		stSRVDesc.Format = texture.pITexture->GetDesc().Format;
		stSRVDesc.ViewDimension = texture.emTextureType;
		stSRVDesc.Texture2D.MipLevels = texture.pITexture->GetDesc().MipLevels;

		GlobalEngine.pID3DDevice->CreateShaderResourceView(
			texture.pITexture.Get(),
			&stSRVDesc,
			pISRVCBVHeap[i]->GetCPUDescriptorHandleForHeapStart());

		//创建CBV描述符
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = pICBVUpload[i]->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = static_cast<UINT>(szConstBuffer);

		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvSrvHandle(
			pISRVCBVHeap[i]->GetCPUDescriptorHandleForHeapStart(),
			1,
			GlobalEngine.pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

		GlobalEngine.pID3DDevice->CreateConstantBufferView(&cbvDesc, cbvSrvHandle);


		const UINT64 fence = GlobalEngine.n64FenceValue;
		GlobalEngine.pICommandQueue->Signal(GlobalEngine.pIFence.Get(), fence);
		GlobalEngine.n64FenceValue++;
		//---------------------------------------------------------------------------------------------
		// 看命令有没有真正执行到围栏标记的这里，没有就利用事件去等待，注意使用的是命令队列对象的指针
		if (GlobalEngine.pIFence->GetCompletedValue() < fence)
		{
			GlobalEngine.pIFence->SetEventOnCompletion(fence, GlobalEngine.hFenceEvent);
			WaitForSingleObject(GlobalEngine.hFenceEvent, INFINITE);
		}

	}
	
	ModelPosition = position;
	ModelRotation = rotation;
	ModelScale = scale;

	TranslationMatrix = XMMatrixTranslation(ModelPosition.x, ModelPosition.y, ModelPosition.z);
	RotationMatrix = XMMatrixRotationRollPitchYaw(ModelRotation.x, ModelRotation.y, ModelRotation.z);
	ScaleMatrix = XMMatrixScaling(ModelScale.x, ModelScale.y, ModelScale.z);

}

inline void ComplexModel::RenderModel(Camera camera, Light light)
{

	for (int i = 0; i < NumMeshes; i++)
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

		XMStoreFloat4x4(&pConstBuffer[i]->m_MVP, xmMVP);
		XMStoreFloat4x4(&pConstBuffer[i]->m_RotationMatrix, RotationMatrix);
		XMStoreFloat4x4(&pConstBuffer[i]->m_ModelMatrix, ModelMatrix);

		XMStoreFloat4(&pConstBuffer[i]->f_LightColor, light.lightColor);
		XMStoreFloat4(&pConstBuffer[i]->f_LightPosition, light.lightPosition);
		XMStoreFloat4(&pConstBuffer[i]->f_LightDirection, light.lightDirection);
		XMStoreFloat4(&pConstBuffer[i]->f_ViewPosition, camera.CameraPosition);

		//pConstBuffer[i]->Ns = 456;
		pConstBuffer[i]->Ns = mesh_mtl[i].Ns;
		//XMVECTOR temp_ka = { 1.0f,0.0f,0.0f};
		//XMStoreFloat3(&pConstBuffer[i]->Ka, temp_ka);
		XMStoreFloat3(&pConstBuffer[i]->Ka, { mesh_mtl[i].Ka[0],mesh_mtl[i].Ka[1] ,mesh_mtl[i].Ka[2] });
		//XMVECTOR temp_kd = { 0.0f,1.0f,0.0f };
		//XMStoreFloat3(&pConstBuffer[i]->Kd, temp_kd);
		XMStoreFloat3(&pConstBuffer[i]->Kd, { mesh_mtl[i].Kd[0],mesh_mtl[i].Kd[1] ,mesh_mtl[i].Kd[2] });
		//XMVECTOR temp_ks = { 0.0f,0.0f,1.0f };
		//XMStoreFloat3(&pConstBuffer[i]->Ks, temp_ks);
		XMStoreFloat3(&pConstBuffer[i]->Ks, { mesh_mtl[i].Ks[0],mesh_mtl[i].Ks[1] ,mesh_mtl[i].Ks[2] });
		//XMVECTOR temp_ke = { 0.5f,0.5f,0.5f };
		//XMStoreFloat3(&pConstBuffer[i]->Ke, temp_ke);
		XMStoreFloat3(&pConstBuffer[i]->Ke, { mesh_mtl[i].Ke[0],mesh_mtl[i].Ke[1] ,mesh_mtl[i].Ke[2] });
		//pConstBuffer[i]->Ni = 666;
		pConstBuffer[i]->Ni = mesh_mtl[i].Ni;
		//pConstBuffer[i]->d = 123;
		pConstBuffer[i]->d = mesh_mtl[i].d;
		//pConstBuffer[i]->illum = 789;
		pConstBuffer[i]->illum = mesh_mtl[i].illum;

		//设置描述符堆
		ID3D12DescriptorHeap* ppHeaps[] = { pISRVCBVHeap[i].Get(),GlobalEngine.pISamplerDescriptorHeap.Get()};
		GlobalEngine.pICommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		//设置SRV
		GlobalEngine.pICommandList->SetGraphicsRootDescriptorTable(
			0,
			pISRVCBVHeap[i]->GetGPUDescriptorHandleForHeapStart());

		CD3DX12_GPU_DESCRIPTOR_HANDLE stGPUCBVHandle(
			pISRVCBVHeap[i]->GetGPUDescriptorHandleForHeapStart(),
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


		GlobalEngine.pICommandList->SetPipelineState(GlobalEngine.pIPipelineState4.Get());


		GlobalEngine.pICommandList->IASetPrimitiveTopology(ModelToplogy[i]);
		GlobalEngine.pICommandList->IASetVertexBuffers(0, 1, &stVertexBufferView[i]);
		GlobalEngine.pICommandList->IASetIndexBuffer(&stIndexBufferView[i]);
		GlobalEngine.pICommandList->DrawIndexedInstanced(nIndicesNum[i], 1, 0, 0, 0);


	}

}

ComplexModel::~ComplexModel()
{
}


TCHAR* ConvertStringToTCHAR(const string& str) {
	int length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	if (length == 0) {
		// 转换失败
		return nullptr;
	}

	TCHAR* tcharString = new TCHAR[length];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, tcharString, length);
	return tcharString;
}


inline void ComplexModel::LoadModelMtl(const char* ModelMtlPath, map<string, Material> *mtl_map)
{
	ifstream file(ModelMtlPath);
	if (!file.is_open()) {
		cout << "Error opening MTL file: " << ModelMtlPath << endl;
		return;
	}

	Material currentMaterial;
	string line;
	while (getline(file, line)) {
		istringstream iss(line);
		string keyword;
		iss >> keyword;

		if (keyword == "newmtl") {
			currentMaterial = Material();
			iss >> currentMaterial.name;

			mtl_map->insert(make_pair(currentMaterial.name, currentMaterial));
		}
		else if (keyword == "Ns") {
			iss >> (*mtl_map)[currentMaterial.name].Ns;
		}
		else if (keyword == "Ka") {
			iss >> (*mtl_map)[currentMaterial.name].Ka[0] >> (*mtl_map)[currentMaterial.name].Ka[1] >> (*mtl_map)[currentMaterial.name].Ka[2];
		}
		else if (keyword == "Kd") {
			iss >> (*mtl_map)[currentMaterial.name].Kd[0] >> (*mtl_map)[currentMaterial.name].Kd[1] >> (*mtl_map)[currentMaterial.name].Kd[2];
		}
		else if (keyword == "Ks") {
			iss >> (*mtl_map)[currentMaterial.name].Ks[0] >> (*mtl_map)[currentMaterial.name].Ks[1] >> (*mtl_map)[currentMaterial.name].Ks[2];
		}
		else if (keyword == "Ke") {
			iss >> (*mtl_map)[currentMaterial.name].Ke[0] >> (*mtl_map)[currentMaterial.name].Ke[1] >> (*mtl_map)[currentMaterial.name].Ke[2];
		}
		else if (keyword == "Ni") {
			iss >> (*mtl_map)[currentMaterial.name].Ni;
		}
		else if (keyword == "d") {
			iss >> (*mtl_map)[currentMaterial.name].d;
		}
		else if (keyword == "illum") {
			iss >> (*mtl_map)[currentMaterial.name].illum;
		}
		else if (keyword == "map_Kd") {
			string texturePath;
			getline(iss, texturePath);
			TCHAR* tcharTexturePath = ConvertStringToTCHAR(texturePath);
			(*mtl_map)[currentMaterial.name].res_type_path.insert(make_pair(map_Kd, tcharTexturePath));
			
		}
		else if (keyword == "map_Ke") {
			string texturePath;
			getline(iss, texturePath);
			TCHAR* tcharTexturePath = ConvertStringToTCHAR(texturePath);
			(*mtl_map)[currentMaterial.name].res_type_path.insert(make_pair(map_Ke, tcharTexturePath));
			
		}
		else if (keyword == "map_Bump") {
			string texturePath;
			getline(iss, texturePath);
			TCHAR* tcharTexturePath = ConvertStringToTCHAR(texturePath);
			(*mtl_map)[currentMaterial.name].res_type_path.insert(make_pair(map_Bump, tcharTexturePath));
			
		}
			
	}
	file.close();
}
