#pragma once

//���嶥��ṹ��
struct ModelMeshVertex
{
    //����ṹ����������ÿ������ķ�����Ϣ����Shader�л���ʱû����
    XMFLOAT3 Position;//����λ����Ϣ��float[3]
    XMFLOAT2 TextureCoordinate;//��������������Ϣ��float[2]
    XMFLOAT3 Normal;//���㷨����Ϣ��float[3]
};

struct DefaultMeshVertex
{
    XMFLOAT3 Position;
    XMFLOAT3 Color;
};

class Mesh
{
public:
	Mesh();
	Mesh(Engine engine);
	void LoadMesh(TCHAR MeshFilePath[]);
    void LoadDefaultMesh();
	~Mesh();

    //һ�������ṩ������������еĶ���buffer��view�������˶���buffer����Ϣ
    D3D12_VERTEX_BUFFER_VIEW stVertexBufferView = {};
    //һ�������ṩ������������еĶ���buffer��view������������buffer����Ϣ
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

inline void Mesh::LoadMesh(TCHAR MeshFilePath[])
{
    const char* modelFilename = "D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Models\\kuroro.obj";
    assert(modelFilename != nullptr);
    const std::string filePath(modelFilename);

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

    /*
    
    float fBoxSize = 1.0f;
    float fTCMax = 2.0f;

    vector<ModelMeshVertex> mesh_vertex = {
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
    vector<int> mesh_index = {
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
    

    ifstream file(MeshFilePath);
    if (file.is_open())
    {
        mesh_vertex.clear();
        mesh_index.clear();

        int vt_no = 0;
        int vn_no = 0;

        string line;
        while (getline(file, line))
        {
            istringstream iss(line);
            string prefix;

            iss >> prefix;

            if (prefix == "v") // ������Ϣ
            {
                ModelMeshVertex vertex = {};
                iss >> vertex.Position.x >> vertex.Position.y >> vertex.Position.z;
                mesh_vertex.push_back(vertex);
            }
            else if (prefix == "vt") // ����������Ϣ
            {
                iss >> mesh_vertex[vt_no].TextureCoordinate.x >> mesh_vertex[vt_no].TextureCoordinate.y;
                vt_no++;
            }
            else if (prefix == "vn") // ������Ϣ
            {
                iss >> mesh_vertex[vn_no].Normal.x >> mesh_vertex[vn_no].Normal.y >> mesh_vertex[vn_no].Normal.z;
                vn_no++;
            }
        }
        file.close();
    }

    */
    const UINT nVertexBufferSize = sizeof(mesh_vertex) * mesh_vertex.size();

    const UINT nIndexBufferSize = sizeof(int) * mesh_index.size();

    nIndicesNum = mesh_index.size();
    nVerticesNum = mesh_vertex.size();
    //�����Դ�ṹ�弰�����ύ��Դ����
    //����������Ϊ�ϴ���
    D3D12_HEAP_PROPERTIES stHeapUpload = { D3D12_HEAP_TYPE_UPLOAD };//Upload�ĸ�����Ǵ�CPU�ڴ��ϴ����Դ��е���˼
    
    //�������㻺��
    D3D12_RESOURCE_DESC stResDescVertex = CD3DX12_RESOURCE_DESC::Buffer(nVertexBufferSize);
    GlobalEngine.pID3DDevice->CreateCommittedResource(
        &stHeapUpload,
        D3D12_HEAP_FLAG_NONE,
        &stResDescVertex,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pIVertexBuffer));
    //�������ļ�Ϊ�õ�Ҫ���ƵĶ���buffer���������潫�������ݴ�CPU�ڴ����Դ���д���
    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    pIVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, &mesh_vertex[0], nVertexBufferSize);
    pIVertexBuffer->Unmap(0, nullptr);

    //��䶥��buffer��ͼ�Ľṹ�壬����GPU����������Դʵ����Vertex Buffer
    stVertexBufferView.BufferLocation = pIVertexBuffer->GetGPUVirtualAddress();
    stVertexBufferView.StrideInBytes = sizeof(ModelMeshVertex);
    stVertexBufferView.SizeInBytes = nVertexBufferSize;

    //������������
    D3D12_RESOURCE_DESC stResDescIndex = CD3DX12_RESOURCE_DESC::Buffer(nIndexBufferSize);
    GlobalEngine.pID3DDevice->CreateCommittedResource(
        &stHeapUpload,
        D3D12_HEAP_FLAG_NONE,
        &stResDescIndex,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pIIndexBuffer));

    //�������ļ�Ϊ�õ�Ҫ���ƵĶ�������buffer���������潫�������ݴ�CPU�ڴ����Դ���д���
    UINT8* pIndexDataBegin = nullptr;
    pIIndexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin));
    memcpy(pIndexDataBegin, &mesh_index[0], nIndexBufferSize);
    pIIndexBuffer->Unmap(0, nullptr);

    //��䶥��buffer��ͼ�Ľṹ�壬����GPU����������Դ��Index Buffer
    stIndexBufferView.BufferLocation = pIIndexBuffer->GetGPUVirtualAddress();
    stIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
    stIndexBufferView.SizeInBytes = nIndexBufferSize;
    
}

inline void Mesh::LoadDefaultMesh()
{
    DefaultMeshVertex coordinate_axis[] = {
        { {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} },//X�ᣬ��ɫ
        { {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} }, //X�ᣬ��ɫ

        { {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },//Y�ᣬ��ɫ
        { {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} }, //Y�ᣬ��ɫ

        { {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f} },//Z�ᣬ��ɫ
        { {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} }  //Z�ᣬ��ɫ
    };

    const UINT nVertexBufferSize = sizeof(coordinate_axis);

    nVerticesNum = _countof(coordinate_axis);

    //�����Դ�ṹ�弰�����ύ��Դ����
    //����������Ϊ�ϴ���
    D3D12_HEAP_PROPERTIES stHeapUpload = { D3D12_HEAP_TYPE_UPLOAD };//Upload�ĸ�����Ǵ�CPU�ڴ��ϴ����Դ��е���˼

    //�������㻺��
    D3D12_RESOURCE_DESC stResDescVertex = CD3DX12_RESOURCE_DESC::Buffer(nVertexBufferSize);
    GlobalEngine.pID3DDevice->CreateCommittedResource(
        &stHeapUpload,
        D3D12_HEAP_FLAG_NONE,
        &stResDescVertex,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pIVertexBuffer));
    //�������ļ�Ϊ�õ�Ҫ���ƵĶ���buffer���������潫�������ݴ�CPU�ڴ����Դ���д���
    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    pIVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, coordinate_axis, nVertexBufferSize);
    pIVertexBuffer->Unmap(0, nullptr);

    //��䶥��buffer��ͼ�Ľṹ�壬����GPU����������Դʵ����Vertex Buffer
    stVertexBufferView.BufferLocation = pIVertexBuffer->GetGPUVirtualAddress();
    stVertexBufferView.StrideInBytes = sizeof(DefaultMeshVertex);
    stVertexBufferView.SizeInBytes = nVertexBufferSize;
}

Mesh::~Mesh()
{
}