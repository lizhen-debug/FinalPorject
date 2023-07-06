#pragma once
class Engine
{
public:
	Engine();
    Engine(HWND hwnd);
	~Engine();

    
    UINT iWidth;
    UINT iHeight;
    //ÿ֡��������������˫����Ϊ2��������Ϊ3�������u�Ǻ�׺�������ֽ���Ϊ�޷�������
    UINT nFrameBackBufCount = 3u;
    //ÿ֡����������������ɼ�ÿ֡����������Ϊ3���������������Ϊ0��1��2
    UINT nFrameIndex = 0;
    //0uָ���ǣ�������0����Ϊ�޷����������ͣ�uΪ��׺
    UINT nDXGIFactoryFlags = 0u;//���ڴ���DX������flag����Ϊ����������Ҫ�ṩһ��flag
    UINT nRTVDescriptorSize = 0u;//���������Ĵ�С
    float fAspectRatio = 3.0f;//���ڿ��ƻ��������δ�С�ı���ֵ
    UINT nSamplerDescriptorSize = 0; //��������С
    
    //Χ���ı��ֵ��0Ϊ���֣�ui64Ϊ��׺����ʾ��0ָ����ֵΪ�޷��� 64 λ��������
    UINT64 n64FenceValue = 0ui64;
    //���ڴ�Ŵ�����event�����ָ��
    HANDLE hFenceEvent = nullptr;
    //�����ṩ������������е�DX�ӿڣ��˴�ֱ�Ӹ�ֵΪ�����Ͻ�λ�ã�0��0�������ڿ�͸�
    CD3DX12_VIEWPORT stViewPort;
    //�����ṩ������������еĲ��о��Σ��������ƿɼ����򣬴˴���С��λ�����ӿ�һ��
    CD3DX12_RECT  stScissorRect;
    //����DX����
    ComPtr<IDXGIFactory5>                pIDXGIFactory5;
    ComPtr<IDXGIAdapter1>                pIAdapter;
    ComPtr<ID3D12Device4>                pID3DDevice;
    ComPtr<ID3D12CommandQueue>           pICommandQueue;
    ComPtr<ID3D12CommandAllocator>       pICommandAllocator;
    ComPtr<ID3D12GraphicsCommandList>    pICommandList;
    ComPtr<IDXGISwapChain1>              pISwapChain1;
    ComPtr<IDXGISwapChain3>              pISwapChain3;
    ComPtr<ID3D12Resource>*              pIARenderTargets;
    ComPtr<ID3D12DescriptorHeap>         pIRTVHeap;
    ComPtr<ID3D12DescriptorHeap>		 pIDSVHeap;
    ComPtr<ID3D12DescriptorHeap>		 pISamplerDescriptorHeap;
    ComPtr<ID3D12RootSignature>          pIRootSignature;
    ComPtr<ID3D12PipelineState>          pIPipelineState1;
    ComPtr<ID3D12PipelineState>          pIPipelineState2;
    ComPtr<ID3D12Fence>                  pIFence;
    ComPtr<ID3D12Resource>				 pIDepthStencilBuffer;

    D3D12_RESOURCE_BARRIER stBeginResBarrier = {};
    D3D12_RESOURCE_BARRIER stEndResBarrier = {};

private:

};

Engine::Engine()
{
}

Engine::Engine(HWND hwnd)
{
    //ʹ��com����ָ��ĳ�ʼ���������õĻ��п��ܳ��ֳ����˳�ʱָ���ͷŲ����ڴ���ʳ�ͻ���쳣
    HRESULT hr = CoInitialize(nullptr);

    // ��ȡ���ھ���
    RECT rect;
    GetWindowRect(hwnd, &rect);
    // ���㴰�ڵĿ�Ⱥ͸߶�
    iWidth = rect.right - rect.left;
    iHeight = rect.bottom - rect.top;

    stViewPort.TopLeftX = 0;
    stViewPort.TopLeftY = 0;
    stViewPort.Width = iWidth;
    stViewPort.Height = iHeight;
    stViewPort.MaxDepth = 1;

    stScissorRect.left = 0;
    stScissorRect.top = 0;
    stScissorRect.right = iWidth;
    stScissorRect.bottom = iHeight;

    //����factory���Ա�Ϊ�������ִ������������豸��
    CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(&pIDXGIFactory5));

    //�ر�ALT+ENTER���л�ȫ���Ĺ��ܣ���Ϊû��ʵ��OnSize���������ȹر�
    pIDXGIFactory5->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

    //ö�ٲ�ѡ��һ�����ʵ���ʾ���������Կ���������D3D�豸�ӿ�
    for (UINT adapterIndex = 0; //����������
        DXGI_ERROR_NOT_FOUND != pIDXGIFactory5->EnumAdapters1(adapterIndex, &pIAdapter); //ö��������
        ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc = {};//��������������ṹ��ı���
        pIAdapter->GetDesc1(&desc);//��ȡ�������ṹ����Ϣ

        //���ڴ˴���ϵ㣬�鿴ѡ���������Կ���Ϊ�����������豸
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)//������������������������
        {
            continue;
        }

        //�����������D3D֧�ֵļ��ݼ�������ֱ��Ҫ��֧��12.1��������ע�ⷵ�ؽӿڵ��Ǹ���������Ϊ��nullptr��
        // �����Ͳ���ʵ�ʴ���һ���豸�ˣ�Ҳ�����ٵ���release���ͷŽӿڡ�����һ����Ҫ�ļ���
        if (SUCCEEDED(D3D12CreateDevice(pIAdapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr)))
        {
            break;
        }
    }
    //����D3D12.1���豸
    D3D12CreateDevice(pIAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&pID3DDevice));

    //----------------------------------------------------------------------------------
    //����DX�������
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    pID3DDevice->CreateCommandQueue(
        &queueDesc,
        IID_PPV_ARGS(&pICommandQueue));

    //���������б������
    pID3DDevice->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&pICommandAllocator));

    //����ͼ�������б�
    pID3DDevice->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        pICommandAllocator.Get(),
        nullptr,
        IID_PPV_ARGS(&pICommandList));

    //��������CPU��GPUͬ��������ͬ������ Χ��
    pID3DDevice->CreateFence(0
        , D3D12_FENCE_FLAG_NONE
        , IID_PPV_ARGS(&pIFence));
    n64FenceValue = 1;//Χ��ֵ

    //����һ��Eventͬ���������ڵȴ�Χ���¼�֪ͨ
    hFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (hFenceEvent == nullptr)
    {
        HRESULT_FROM_WIN32(GetLastError());
    }

    //����������
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};//�����������ṹ�����
    //��д�������ṹ��
    swapChainDesc.BufferCount = nFrameBackBufCount;
    swapChainDesc.Width = iWidth;
    swapChainDesc.Height = iHeight;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;
    //Ϊ���ھ������������
    pIDXGIFactory5->CreateSwapChainForHwnd(
        pICommandQueue.Get(),
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &pISwapChain1//�˴�ֻ��ʹ�õͰ汾������
    );

    //�õ���ǰ�󻺳�������ţ�Ҳ������һ����Ҫ������ʾ�Ļ����������
    //As������Ϊ��ʹ�õͰ汾��SwapChain1�ӿڵõ��߰汾��SwapChain3�ӿڣ�������ָ��com��һ�ַ���
    //Ŀ����Ϊ�˵���GetCurrentBackBufferIndex����
    pISwapChain1.As(&pISwapChain3);
    //ע��˴�ʹ���˸߰汾��SwapChain3�ӿڵĺ���
    nFrameIndex = pISwapChain3->GetCurrentBackBufferIndex();

    //----------------------------------------------------------------------------------
    //����RTV��������
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = nFrameBackBufCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    pID3DDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&pIRTVHeap));
    //�õ�ÿ��������Ԫ�صĴ�С�������������ʱƫ�Ƶ�����
    nRTVDescriptorSize = pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    //����RTV������
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pIRTVHeap->GetCPUDescriptorHandleForHeapStart());
    pIARenderTargets = new ComPtr<ID3D12Resource>[nFrameBackBufCount];
    //����rtvHandle�������Ϊ������������Ϊ����forѭ������RTV
    for (UINT i = 0; i < nFrameBackBufCount; i++)
    {
        pISwapChain3->GetBuffer(i, IID_PPV_ARGS(&pIARenderTargets[i]));
        pID3DDevice->CreateRenderTargetView(pIARenderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, nRTVDescriptorSize);//������offset
    }

    //������Ȼ��弰��Ȼ�����������
    D3D12_CLEAR_VALUE stDepthOptimizedClearValue = {};
    stDepthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    stDepthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    stDepthOptimizedClearValue.DepthStencil.Stencil = 0;


    CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC tex2D = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D24_UNORM_S8_UINT, iWidth, iHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    //ʹ����ʽĬ�϶Ѵ���һ��������建������
    //��Ϊ��������Ȼ�������һֱ��ʹ�ã����õ����岻������ֱ��ʹ����ʽ�ѣ�ͼ����
    pID3DDevice->CreateCommittedResource(
        &heapProperties
        , D3D12_HEAP_FLAG_NONE
        , &tex2D
        , D3D12_RESOURCE_STATE_DEPTH_WRITE
        , &stDepthOptimizedClearValue
        , IID_PPV_ARGS(&pIDepthStencilBuffer)
    );

    D3D12_DEPTH_STENCIL_VIEW_DESC stDepthStencilDesc = {};
    stDepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    stDepthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    stDepthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    pID3DDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&pIDSVHeap));

    pID3DDevice->CreateDepthStencilView(pIDepthStencilBuffer.Get()
        , &stDepthStencilDesc
        , pIDSVHeap->GetCPUDescriptorHandleForHeapStart());
 
    //----------------------------------------------------------------------------------
    //��ǩ���Ĵ���
    //������������
    D3D12_FEATURE_DATA_ROOT_SIGNATURE stFeatureData = {};
    // ����Ƿ�֧��V1.1�汾�ĸ�ǩ��
    stFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(pID3DDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &stFeatureData, sizeof(stFeatureData))))
    {
        stFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }
    //��̬�������汾------------------------------------------------------------------------------
    CD3DX12_DESCRIPTOR_RANGE1 stDSPRanges[3] = {};
    stDSPRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE);
    stDSPRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
    stDSPRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

    CD3DX12_ROOT_PARAMETER1 stRootParameters[3] = {};
    stRootParameters[0].InitAsDescriptorTable(1, &stDSPRanges[0], D3D12_SHADER_VISIBILITY_PIXEL);//SRV��PS�ɼ�
    stRootParameters[1].InitAsDescriptorTable(1, &stDSPRanges[1], D3D12_SHADER_VISIBILITY_ALL); //CBV������Shader�ɼ�
    stRootParameters[2].InitAsDescriptorTable(1, &stDSPRanges[2], D3D12_SHADER_VISIBILITY_PIXEL);//SAMPLE��PS�ɼ�

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC stRootSignatureDesc = {};

    stRootSignatureDesc.Init_1_1(
        _countof(stRootParameters),
        stRootParameters,
        0,
        nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    //------------------------------------------------------------------------------

    ComPtr<ID3DBlob> pISignatureBlob;
    ComPtr<ID3DBlob> pIErrorBlob;
    //ʹ�ø߰汾�����¼��ݵķ����������ǩ��
    D3DX12SerializeVersionedRootSignature(
        &stRootSignatureDesc,
        stFeatureData.HighestVersion,
        &pISignatureBlob,
        &pIErrorBlob);

    pID3DDevice->CreateRootSignature(
        0,
        pISignatureBlob->GetBufferPointer(),
        pISignatureBlob->GetBufferSize(),
        IID_PPV_ARGS(&pIRootSignature));

    //---------------------------------------------------------------------------------------------

    //����Shader
    ComPtr<ID3DBlob> vertexShader1;
    ComPtr<ID3DBlob> pixelShader1;

    ComPtr<ID3DBlob> vertexShader2;
    ComPtr<ID3DBlob> pixelShader2;

    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

    TCHAR pszShaderFileName1[] = _T("D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Shaders\\texture_cube.hlsl");

    D3DCompileFromFile(pszShaderFileName1, nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader1, nullptr);

    D3DCompileFromFile(pszShaderFileName1, nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader1, nullptr);

    TCHAR pszShaderFileName2[] = _T("D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Shaders\\shaders.hlsl");

    D3DCompileFromFile(pszShaderFileName2, nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader2, nullptr);

    D3DCompileFromFile(pszShaderFileName2, nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader2, nullptr);


    //������������������벼�ֵĽṹ��
    D3D12_INPUT_ELEMENT_DESC inputElementDescs1[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    D3D12_INPUT_ELEMENT_DESC inputElementDescs2[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    CD3DX12_RASTERIZER_DESC rasterizerDesc = {};//��դ���������ṹ��
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;//�������ģʽ\�߿�ģʽ
    rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;//�����޳�ģʽ

    //������Ⱦ����״̬����ӿ�1
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc1 = {};
    psoDesc1.InputLayout = { inputElementDescs1, _countof(inputElementDescs1) };//_countof�꣬�����ȡ��̬����Ԫ�ظ���
    psoDesc1.pRootSignature = pIRootSignature.Get();
    psoDesc1.VS = CD3DX12_SHADER_BYTECODE(vertexShader1.Get());
    psoDesc1.PS = CD3DX12_SHADER_BYTECODE(pixelShader1.Get());
    psoDesc1.RasterizerState = rasterizerDesc;
    psoDesc1.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc1.DepthStencilState.StencilEnable = FALSE;
    psoDesc1.DepthStencilState.DepthEnable = TRUE;			//����Ȼ���				
    psoDesc1.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//������Ȼ���д�빦��
    psoDesc1.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;     //��Ȳ��Ժ�������ֵΪ��ͨ����Ȳ��ԣ�����Сֵд�룩
    psoDesc1.SampleMask = UINT_MAX;
    psoDesc1.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc1.NumRenderTargets = 1;
    psoDesc1.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc1.SampleDesc.Count = 1;
    pID3DDevice->CreateGraphicsPipelineState(&psoDesc1, IID_PPV_ARGS(&pIPipelineState1));

    //������Ⱦ����״̬����ӿ�2
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc2 = {};
    psoDesc2.InputLayout = { inputElementDescs2, _countof(inputElementDescs2) };//_countof�꣬�����ȡ��̬����Ԫ�ظ���
    psoDesc2.pRootSignature = pIRootSignature.Get();
    psoDesc2.VS = CD3DX12_SHADER_BYTECODE(vertexShader2.Get());
    psoDesc2.PS = CD3DX12_SHADER_BYTECODE(pixelShader2.Get());
    psoDesc2.RasterizerState = rasterizerDesc;
    psoDesc2.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc2.DepthStencilState.StencilEnable = FALSE;
    psoDesc2.DepthStencilState.DepthEnable = TRUE;			
    psoDesc2.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    psoDesc2.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    psoDesc2.SampleMask = UINT_MAX;
    psoDesc2.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc2.NumRenderTargets = 1;
    psoDesc2.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc2.SampleDesc.Count = 1;
    pID3DDevice->CreateGraphicsPipelineState(&psoDesc2, IID_PPV_ARGS(&pIPipelineState2));

    //���������������ѽṹ��
    D3D12_DESCRIPTOR_HEAP_DESC stSamplerHeapDesc = {};
    stSamplerHeapDesc.NumDescriptors = nSampleMaxCnt;
    stSamplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    stSamplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    //������������
    pID3DDevice->CreateDescriptorHeap(
        &stSamplerHeapDesc,
        IID_PPV_ARGS(&pISamplerDescriptorHeap));
    //��ȡ�������ѵĴ�С�������������offset����ʽ�������������
    nSamplerDescriptorSize = pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    CD3DX12_CPU_DESCRIPTOR_HANDLE hSamplerHeap(pISamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    D3D12_SAMPLER_DESC stSamplerDesc = {};
    stSamplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
    stSamplerDesc.MinLOD = 0;
    stSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    stSamplerDesc.MipLODBias = 0.0f;
    stSamplerDesc.MaxAnisotropy = D3D12_MAX_MAXANISOTROPY;
    stSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    // Sampler 1
    stSamplerDesc.BorderColor[0] = 0.0f;
    stSamplerDesc.BorderColor[1] = 0.0f;
    stSamplerDesc.BorderColor[2] = 0.0f;
    stSamplerDesc.BorderColor[3] = 1.0f;
    stSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    stSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    stSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    pID3DDevice->CreateSampler(&stSamplerDesc, hSamplerHeap);

    hSamplerHeap.Offset(nSamplerDescriptorSize);

    // Sampler 2
    stSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    stSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    stSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    pID3DDevice->CreateSampler(&stSamplerDesc, hSamplerHeap);

    hSamplerHeap.Offset(nSamplerDescriptorSize);

    // Sampler 3
    stSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    stSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    stSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    pID3DDevice->CreateSampler(&stSamplerDesc, hSamplerHeap);

    hSamplerHeap.Offset(nSamplerDescriptorSize);

    // Sampler 4
    stSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
    stSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
    stSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
    pID3DDevice->CreateSampler(&stSamplerDesc, hSamplerHeap);

    hSamplerHeap.Offset(nSamplerDescriptorSize);

    // Sampler 5
    stSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
    stSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
    stSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
    pID3DDevice->CreateSampler(&stSamplerDesc, hSamplerHeap);

    //���ÿ֡��Ⱦ��ʼ�ͽ���������Դ���Ͻṹ��
    stBeginResBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    stBeginResBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    stBeginResBarrier.Transition.pResource = pIARenderTargets[nFrameIndex].Get();
    stBeginResBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    stBeginResBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    stBeginResBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    
    stEndResBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    stEndResBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    stEndResBarrier.Transition.pResource = pIARenderTargets[nFrameIndex].Get();
    stEndResBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    stEndResBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    stEndResBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
}

Engine::~Engine()
{
}