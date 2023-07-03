#pragma once

//定义顶点结构体
struct ST_GRS_VERTEX
{
    //顶点结构体额外加入了每个顶点的法线信息，但Shader中还暂时没有用
    XMFLOAT3 m_vPos;//顶点位置信息，float[3]
    XMFLOAT2 m_vTex;//顶点纹理坐标信息，float[2]
    XMFLOAT3 m_vNor;//顶点法线信息，float[3]
};

class Mesh
{
public:
	Mesh();
	Mesh(Engine engine);
	void LoadMesh(TCHAR MeshFilePath[]);
	~Mesh();

    //一个用于提供给绘制命令队列的顶点buffer的view，描述了顶点buffer的信息
    D3D12_VERTEX_BUFFER_VIEW stVertexBufferView = {};
    //一个用于提供给绘制命令队列的顶点buffer的view，描述了索引buffer的信息
    D3D12_INDEX_BUFFER_VIEW stIndexBufferView = {};
    UINT nIndicesNum = 0;

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

inline void Mesh::LoadMesh(TCHAR MeshFilePath[])
{
    
    float fBoxSize = 2.0f;
    float fTCMax = 2.0f;
    //定义正方体的3D数据结构，注意此处的纹理坐标故意设置为大于1
    ST_GRS_VERTEX stCubeVertices[] = {
        { {-1.0f * fBoxSize,  1.0f * fBoxSize, -1.0f * fBoxSize}, {0.0f * fTCMax, 0.0f * fTCMax}, {0.0f,  0.0f, -1.0f} },
        { {1.0f * fBoxSize,  1.0f * fBoxSize, -1.0f * fBoxSize}, {1.0f * fTCMax, 0.0f * fTCMax},  {0.0f,  0.0f, -1.0f} },
        { {-1.0f * fBoxSize, -1.0f * fBoxSize, -1.0f * fBoxSize}, {0.0f * fTCMax, 1.0f * fTCMax}, {0.0f,  0.0f, -1.0f} },
        { {-1.0f * fBoxSize, -1.0f * fBoxSize, -1.0f * fBoxSize}, {0.0f * fTCMax, 1.0f * fTCMax}, {0.0f,  0.0f, -1.0f} },
        { {1.0f * fBoxSize,  1.0f * fBoxSize, -1.0f * fBoxSize}, {1.0f * fTCMax, 0.0f * fTCMax},  {0.0f, 0.0f, -1.0f} },
        { {1.0f * fBoxSize, -1.0f * fBoxSize, -1.0f * fBoxSize}, {1.0f * fTCMax, 1.0f * fTCMax},  {0.0f,  0.0f, -1.0f} },
        { {1.0f * fBoxSize,  1.0f * fBoxSize, -1.0f * fBoxSize}, {0.0f * fTCMax, 0.0f * fTCMax},  {1.0f,  0.0f,  0.0f} },
        { {1.0f * fBoxSize,  1.0f * fBoxSize,  1.0f * fBoxSize}, {1.0f * fTCMax, 0.0f * fTCMax},  {1.0f,  0.0f,  0.0f} },
        { {1.0f * fBoxSize, -1.0f * fBoxSize, -1.0f * fBoxSize}, {0.0f * fTCMax, 1.0f * fTCMax},  {1.0f,  0.0f,  0.0f} },
        { {1.0f * fBoxSize, -1.0f * fBoxSize, -1.0f * fBoxSize}, {0.0f * fTCMax, 1.0f * fTCMax},  {1.0f,  0.0f,  0.0f} },
        { {1.0f * fBoxSize,  1.0f * fBoxSize,  1.0f * fBoxSize}, {1.0f * fTCMax, 0.0f * fTCMax},  {1.0f,  0.0f,  0.0f} },
        { {1.0f * fBoxSize, -1.0f * fBoxSize,  1.0f * fBoxSize}, {1.0f * fTCMax, 1.0f * fTCMax},  {1.0f,  0.0f,  0.0f} },
        { {1.0f * fBoxSize,  1.0f * fBoxSize,  1.0f * fBoxSize}, {0.0f * fTCMax, 0.0f * fTCMax},  {0.0f,  0.0f,  1.0f} },
        { {-1.0f * fBoxSize,  1.0f * fBoxSize,  1.0f * fBoxSize}, {1.0f * fTCMax, 0.0f * fTCMax},  {0.0f,  0.0f,  1.0f} },
        { {1.0f * fBoxSize, -1.0f * fBoxSize,  1.0f * fBoxSize}, {0.0f * fTCMax, 1.0f * fTCMax}, {0.0f,  0.0f,  1.0f} },
        { {1.0f * fBoxSize, -1.0f * fBoxSize,  1.0f * fBoxSize}, {0.0f * fTCMax, 1.0f * fTCMax},  {0.0f,  0.0f,  1.0f} },
        { {-1.0f * fBoxSize,  1.0f * fBoxSize,  1.0f * fBoxSize}, {1.0f * fTCMax, 0.0f * fTCMax},  {0.0f,  0.0f,  1.0f} },
        { {-1.0f * fBoxSize, -1.0f * fBoxSize,  1.0f * fBoxSize}, {1.0f * fTCMax, 1.0f * fTCMax},  {0.0f,  0.0f,  1.0f} },
        { {-1.0f * fBoxSize,  1.0f * fBoxSize,  1.0f * fBoxSize}, {0.0f * fTCMax, 0.0f * fTCMax}, {-1.0f,  0.0f,  0.0f} },
        { {-1.0f * fBoxSize,  1.0f * fBoxSize, -1.0f * fBoxSize}, {1.0f * fTCMax, 0.0f * fTCMax}, {-1.0f,  0.0f,  0.0f} },
        { {-1.0f * fBoxSize, -1.0f * fBoxSize,  1.0f * fBoxSize}, {0.0f * fTCMax, 1.0f * fTCMax}, {-1.0f,  0.0f,  0.0f} },
        { {-1.0f * fBoxSize, -1.0f * fBoxSize,  1.0f * fBoxSize}, {0.0f * fTCMax, 1.0f * fTCMax}, {-1.0f,  0.0f,  0.0f} },
        { {-1.0f * fBoxSize,  1.0f * fBoxSize, -1.0f * fBoxSize}, {1.0f * fTCMax, 0.0f * fTCMax}, {-1.0f,  0.0f,  0.0f} },
        { {-1.0f * fBoxSize, -1.0f * fBoxSize, -1.0f * fBoxSize}, {1.0f * fTCMax, 1.0f * fTCMax}, {-1.0f,  0.0f,  0.0f} },
        { {-1.0f * fBoxSize,  1.0f * fBoxSize,  1.0f * fBoxSize}, {0.0f * fTCMax, 0.0f * fTCMax},  {0.0f,  1.0f,  0.0f} },
        { {1.0f * fBoxSize,  1.0f * fBoxSize,  1.0f * fBoxSize}, {1.0f * fTCMax, 0.0f * fTCMax},  {0.0f,  1.0f,  0.0f} },
        { {-1.0f * fBoxSize,  1.0f * fBoxSize, -1.0f * fBoxSize}, {0.0f * fTCMax, 1.0f * fTCMax},  {0.0f,  1.0f,  0.0f} },
        { {-1.0f * fBoxSize,  1.0f * fBoxSize, -1.0f * fBoxSize}, {0.0f * fTCMax, 1.0f * fTCMax},  {0.0f,  1.0f,  0.0f} },
        { {1.0f * fBoxSize,  1.0f * fBoxSize,  1.0f * fBoxSize}, {1.0f * fTCMax, 0.0f * fTCMax},  {0.0f,  1.0f,  0.0f} },
        { {1.0f * fBoxSize,  1.0f * fBoxSize, -1.0f * fBoxSize}, {1.0f * fTCMax, 1.0f * fTCMax},  {0.0f,  1.0f,  0.0f }},
        { {-1.0f * fBoxSize, -1.0f * fBoxSize, -1.0f * fBoxSize}, {0.0f * fTCMax, 0.0f * fTCMax},  {0.0f, -1.0f,  0.0f} },
        { {1.0f * fBoxSize, -1.0f * fBoxSize, -1.0f * fBoxSize}, {1.0f * fTCMax, 0.0f * fTCMax},  {0.0f, -1.0f,  0.0f} },
        { {-1.0f * fBoxSize, -1.0f * fBoxSize,  1.0f * fBoxSize}, {0.0f * fTCMax, 1.0f * fTCMax},  {0.0f, -1.0f,  0.0f} },
        { {-1.0f * fBoxSize, -1.0f * fBoxSize,  1.0f * fBoxSize}, {0.0f * fTCMax, 1.0f * fTCMax},  {0.0f, -1.0f,  0.0f} },
        { {1.0f * fBoxSize, -1.0f * fBoxSize, -1.0f * fBoxSize}, {1.0f * fTCMax, 0.0f * fTCMax},  {0.0f, -1.0f,  0.0f} },
        { {1.0f * fBoxSize, -1.0f * fBoxSize,  1.0f * fBoxSize}, {1.0f * fTCMax, 1.0f * fTCMax},  {0.0f, -1.0f,  0.0f} },
    };

    const UINT nVertexBufferSize = sizeof(stCubeVertices);

    UINT32 pBoxIndices[] //立方体索引数组
        = {
        0,1,2,
        3,4,5,

        6,7,8,
        9,10,11,

        12,13,14,
        15,16,17,

        18,19,20,
        21,22,23,

        24,25,26,
        27,28,29,

        30,31,32,
        33,34,35,
    };

    const UINT nIndexBufferSize = sizeof(pBoxIndices);
    nIndicesNum = _countof(pBoxIndices);
    
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
    memcpy(pVertexDataBegin, stCubeVertices, nVertexBufferSize);
    pIVertexBuffer->Unmap(0, nullptr);

    //填充顶点buffer视图的结构体，告诉GPU被描述的资源实际是Vertex Buffer
    stVertexBufferView.BufferLocation = pIVertexBuffer->GetGPUVirtualAddress();
    stVertexBufferView.StrideInBytes = sizeof(ST_GRS_VERTEX);
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
    memcpy(pIndexDataBegin, pBoxIndices, nIndexBufferSize);
    pIIndexBuffer->Unmap(0, nullptr);

    //填充顶点buffer视图的结构体，告诉GPU被描述的资源是Index Buffer
    stIndexBufferView.BufferLocation = pIIndexBuffer->GetGPUVirtualAddress();
    stIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
    stIndexBufferView.SizeInBytes = nIndexBufferSize;
    
}

Mesh::~Mesh()
{
}