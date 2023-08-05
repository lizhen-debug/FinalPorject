#pragma once

//定义顶点结构体
struct ModelMeshVertex
{
    //顶点结构体额外加入了每个顶点的法线信息，但Shader中还暂时没有用
    XMFLOAT3 Position;//顶点位置信息，float[3]
    XMFLOAT2 TextureCoordinate;//顶点纹理坐标信息，float[2]
    XMFLOAT3 Normal;//顶点法线信息，float[3]
};

struct DefaultMeshVertex
{
    XMFLOAT3 Position;
    XMFLOAT3 Color;
};

struct CubeMeshVertex
{
    XMFLOAT4 Position;
};

class Mesh
{
public:
	Mesh();
	Mesh(Engine engine);
	void LoadMesh(const char* MeshFilePath);
    void LoadDefaultMesh();
    void LoadSkyBoxMesh();
	~Mesh();

    //一个用于提供给绘制命令队列的顶点buffer的view，描述了顶点buffer的信息
    D3D12_VERTEX_BUFFER_VIEW stVertexBufferView = {};
    //一个用于提供给绘制命令队列的顶点buffer的view，描述了索引buffer的信息
    D3D12_INDEX_BUFFER_VIEW stIndexBufferView = {};
    UINT nIndicesNum = 0;
    UINT nVerticesNum = 0;

private:
	Engine GlobalEngine;
    ComPtr<ID3D12Resource>				 pIVertexBuffer;
    ComPtr<ID3D12Resource>				 pIIndexBuffer;
};

Mesh::Mesh()
{
}

Mesh::Mesh(Engine engine)
{
	GlobalEngine = engine;
}

inline void Mesh::LoadMesh(const char* MeshFilePath)
{
    assert(MeshFilePath != nullptr);
    const std::string filePath(MeshFilePath);

    Assimp::Importer importer;
    const std::uint32_t flags{ aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded };
    const aiScene* scene{ importer.ReadFile(filePath.c_str(), flags) };
    assert(scene != nullptr);

    assert(scene->HasMeshes());

    vector<ModelMeshVertex> mesh_vertex;
    vector<int> mesh_index;

    for (std::uint32_t i = 0U; i < scene->mNumMeshes; ++i)
    {
        aiMesh* mesh{ scene->mMeshes[i] };
        assert(mesh != nullptr);

        {
            // Positions
            const std::size_t numVertices{ mesh->mNumVertices };
            assert(numVertices > 0U);
            mesh_vertex.resize(numVertices);
            for (std::uint32_t i = 0U; i < numVertices; ++i)
            {
                mesh_vertex[i].Position = XMFLOAT3(reinterpret_cast<const float*>(&mesh->mVertices[i]));
                mesh_vertex[i].TextureCoordinate = XMFLOAT2(reinterpret_cast<const float*>(&mesh->mTextureCoords[0][i]));
                mesh_vertex[i].Normal= XMFLOAT3(reinterpret_cast<const float*>(&mesh->mNormals[i]));
            }

            // Indices
            const std::uint32_t numFaces{ mesh->mNumFaces };
            assert(numFaces > 0U);
            for (std::uint32_t i = 0U; i < numFaces; ++i)
            {
                const aiFace* face = &mesh->mFaces[i];
                assert(face != nullptr);
                
                assert(face->mNumIndices == 3U);

                mesh_index.push_back(face->mIndices[0U]);
                mesh_index.push_back(face->mIndices[1U]);
                mesh_index.push_back(face->mIndices[2U]);
                
            }
        }
    }

    const UINT nVertexBufferSize = sizeof(mesh_vertex) * mesh_vertex.size();

    const UINT nIndexBufferSize = sizeof(int) * mesh_index.size();

    nIndicesNum = mesh_index.size();
    nVerticesNum = mesh_vertex.size();
    //填充资源结构体及创建提交资源对象
    //堆属性设置为上传堆
    D3D12_HEAP_PROPERTIES stHeapUpload = { D3D12_HEAP_TYPE_UPLOAD };//Upload的概念就是从CPU内存上传到显存中的意思
    
    //创建顶点缓冲
    D3D12_RESOURCE_DESC stResDescVertex = CD3DX12_RESOURCE_DESC::Buffer(nVertexBufferSize);
    GlobalEngine.pID3DDevice->CreateCommittedResource(
        &stHeapUpload,
        D3D12_HEAP_FLAG_NONE,
        &stResDescVertex,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pIVertexBuffer));
    //上述核心即为得到要绘制的顶点buffer，用于下面将绘制数据从CPU内存向显存进行传递
    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    pIVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, &mesh_vertex[0], nVertexBufferSize);
    pIVertexBuffer->Unmap(0, nullptr);

    //填充顶点buffer视图的结构体，告诉GPU被描述的资源实际是Vertex Buffer
    stVertexBufferView.BufferLocation = pIVertexBuffer->GetGPUVirtualAddress();
    stVertexBufferView.StrideInBytes = sizeof(ModelMeshVertex);
    stVertexBufferView.SizeInBytes = nVertexBufferSize;

    //创建索引缓冲
    D3D12_RESOURCE_DESC stResDescIndex = CD3DX12_RESOURCE_DESC::Buffer(nIndexBufferSize);
    GlobalEngine.pID3DDevice->CreateCommittedResource(
        &stHeapUpload,
        D3D12_HEAP_FLAG_NONE,
        &stResDescIndex,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pIIndexBuffer));

    //上述核心即为得到要绘制的顶点索引buffer，用于下面将绘制数据从CPU内存向显存进行传递
    UINT8* pIndexDataBegin = nullptr;
    pIIndexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin));
    memcpy(pIndexDataBegin, &mesh_index[0], nIndexBufferSize);
    pIIndexBuffer->Unmap(0, nullptr);

    //填充顶点buffer视图的结构体，告诉GPU被描述的资源是Index Buffer
    stIndexBufferView.BufferLocation = pIIndexBuffer->GetGPUVirtualAddress();
    stIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
    stIndexBufferView.SizeInBytes = nIndexBufferSize;
    
}

inline void Mesh::LoadDefaultMesh()
{
    DefaultMeshVertex coordinate_axis[] = {
        { {-100.0f, 0.0f, 0.0f}, {100.0f, 0.0f, 0.0f} },//X轴，红色
        { {100.0f, 0.0f, 0.0f}, {100.0f, 0.0f, 0.0f} }, //X轴，红色

        { {0.0f, -100.0f, 0.0f}, {0.0f, 100.0f, 0.0f} },//Y轴，蓝色
        { {0.0f, 100.0f, 0.0f}, {0.0f, 100.0f, 0.0f} }, //Y轴，蓝色

        { {0.0f, 0.0f, -100.0f}, {0.0f, 0.0f, 100.0f} },//Z轴，绿色
        { {0.0f, 0.0f, 100.0f}, {0.0f, 0.0f, 100.0f} }  //Z轴，绿色
    };

    const UINT nVertexBufferSize = sizeof(coordinate_axis);

    nVerticesNum = _countof(coordinate_axis);

    //填充资源结构体及创建提交资源对象
    //堆属性设置为上传堆
    D3D12_HEAP_PROPERTIES stHeapUpload = { D3D12_HEAP_TYPE_UPLOAD };//Upload的概念就是从CPU内存上传到显存中的意思

    //创建顶点缓冲
    D3D12_RESOURCE_DESC stResDescVertex = CD3DX12_RESOURCE_DESC::Buffer(nVertexBufferSize);
    GlobalEngine.pID3DDevice->CreateCommittedResource(
        &stHeapUpload,
        D3D12_HEAP_FLAG_NONE,
        &stResDescVertex,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pIVertexBuffer));
    //上述核心即为得到要绘制的顶点buffer，用于下面将绘制数据从CPU内存向显存进行传递
    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    pIVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, coordinate_axis, nVertexBufferSize);
    pIVertexBuffer->Unmap(0, nullptr);

    //填充顶点buffer视图的结构体，告诉GPU被描述的资源实际是Vertex Buffer
    stVertexBufferView.BufferLocation = pIVertexBuffer->GetGPUVirtualAddress();
    stVertexBufferView.StrideInBytes = sizeof(DefaultMeshVertex);
    stVertexBufferView.SizeInBytes = nVertexBufferSize;
}

inline void Mesh::LoadSkyBoxMesh()
{
    float fHighW = -1.0f - (1.0f / (float)GlobalEngine.iWidth);
    float fHighH = -1.0f - (1.0f / (float)GlobalEngine.iHeight);
    float fLowW = 1.0f + (1.0f / (float)GlobalEngine.iWidth);
    float fLowH = 1.0f + (1.0f / (float)GlobalEngine.iHeight);

    CubeMeshVertex stSkyboxVertices[4] = {};

    stSkyboxVertices[0].Position = XMFLOAT4(fLowW, fLowH, 1.0f, 1.0f);
    stSkyboxVertices[1].Position = XMFLOAT4(fLowW, fHighH, 1.0f, 1.0f);
    stSkyboxVertices[2].Position = XMFLOAT4(fHighW, fLowH, 1.0f, 1.0f);
    stSkyboxVertices[3].Position = XMFLOAT4(fHighW, fHighH, 1.0f, 1.0f);

    const UINT nVertexBufferSize = sizeof(stSkyboxVertices);
    nVerticesNum = _countof(stSkyboxVertices);

    //填充资源结构体及创建提交资源对象
    D3D12_RESOURCE_DESC stBufResDesc = {};
    stBufResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    stBufResDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    stBufResDesc.Width = nVertexBufferSize;
    stBufResDesc.Height = 1;
    stBufResDesc.DepthOrArraySize = 1;
    stBufResDesc.MipLevels = 1;
    stBufResDesc.Format = DXGI_FORMAT_UNKNOWN;
    stBufResDesc.SampleDesc.Count = 1;
    stBufResDesc.SampleDesc.Quality = 0;
    stBufResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    stBufResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    //堆属性设置为上传堆
    D3D12_HEAP_PROPERTIES stHeapUpload = { D3D12_HEAP_TYPE_UPLOAD };//Upload的概念就是从CPU内存上传到显存中的意思

    //创建顶点缓冲
    GlobalEngine.pID3DDevice->CreateCommittedResource(
        &stHeapUpload,
        D3D12_HEAP_FLAG_NONE,
        &stBufResDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pIVertexBuffer));
    //上述核心即为得到要绘制的顶点buffer，用于下面将绘制数据从CPU内存向显存进行传递
    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    pIVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, stSkyboxVertices, nVertexBufferSize);
    pIVertexBuffer->Unmap(0, nullptr);

    //填充顶点buffer视图的结构体，告诉GPU被描述的资源实际是Vertex Buffer
    stVertexBufferView.BufferLocation = pIVertexBuffer->GetGPUVirtualAddress();
    stVertexBufferView.StrideInBytes = sizeof(CubeMeshVertex);
    stVertexBufferView.SizeInBytes = nVertexBufferSize;
}

Mesh::~Mesh()
{
}