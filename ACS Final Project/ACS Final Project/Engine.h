#pragma once
class Engine
{
public:
	Engine();
    Engine(HWND hwnd);
	~Engine();

    
    UINT iWidth;
    UINT iHeight;
    //每帧缓冲区的数量，双缓冲为2，三缓冲为3，后面的u是后缀，将数字解释为无符号整型
    UINT nFrameBackBufCount = 3u;
    //每帧缓冲区索引，上面可见每帧缓冲区数量为3，则这里的索引即为0，1，2
    UINT nFrameIndex = 0;
    //0u指的是：将数字0解释为无符号整数类型，u为后缀
    UINT nDXGIFactoryFlags = 0u;//用于创建DX工厂的flag，因为创建工厂需要提供一个flag
    UINT nRTVDescriptorSize = 0u;//堆描述符的大小
    float fAspectRatio = 3.0f;//用于控制绘制三角形大小的比例值
    UINT nSamplerDescriptorSize = 0; //采样器大小
    
    //围栏的标记值，0为数字，ui64为后缀，表示将0指定数值为无符号 64 位整数类型
    UINT64 n64FenceValue = 0ui64;
    //用于存放创建的event对象的指针
    HANDLE hFenceEvent = nullptr;
    //用于提供给绘制命令队列的DX视口，此处直接赋值为：左上角位置（0，0），窗口宽和高
    CD3DX12_VIEWPORT stViewPort;
    //用于提供给绘制命令队列的裁切矩形，用于限制可见区域，此处大小和位置与视口一致
    CD3DX12_RECT  stScissorRect;
    //声明DX变量
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
    //使用com智能指针的初始化，不调用的话有可能出现程序退出时指针释放产生内存访问冲突的异常
    HRESULT hr = CoInitialize(nullptr);

    // 获取窗口矩形
    RECT rect;
    GetWindowRect(hwnd, &rect);
    // 计算窗口的宽度和高度
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

    //创建factory，以便为后续部分创建适配器，设备等
    CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(&pIDXGIFactory5));

    //关闭ALT+ENTER键切换全屏的功能，因为没有实现OnSize处理，所以先关闭
    pIDXGIFactory5->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

    //枚举并选择一个合适的显示适配器（显卡）来创建D3D设备接口
    for (UINT adapterIndex = 0; //适配器索引
        DXGI_ERROR_NOT_FOUND != pIDXGIFactory5->EnumAdapters1(adapterIndex, &pIAdapter); //枚举适配器
        ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc = {};//声明存放适配器结构体的变量
        pIAdapter->GetDesc1(&desc);//获取适配器结构体信息

        //可在此处打断点，查看选择了哪张显卡作为适配器创建设备
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)//如果是软件虚拟适配器，跳过
        {
            continue;
        }

        //检查适配器对D3D支持的兼容级别，这里直接要求支持12.1的能力，注意返回接口的那个参数被置为了nullptr，
        // 这样就不会实际创建一个设备了，也不用再调用release来释放接口。这是一个重要的技巧
        if (SUCCEEDED(D3D12CreateDevice(pIAdapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr)))
        {
            break;
        }
    }
    //创建D3D12.1的设备
    D3D12CreateDevice(pIAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&pID3DDevice));

    //----------------------------------------------------------------------------------
    //创建DX命令队列
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    pID3DDevice->CreateCommandQueue(
        &queueDesc,
        IID_PPV_ARGS(&pICommandQueue));

    //创建命令列表分配器
    pID3DDevice->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&pICommandAllocator));

    //创建图形命令列表
    pID3DDevice->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        pICommandAllocator.Get(),
        nullptr,
        IID_PPV_ARGS(&pICommandList));

    //创建控制CPU和GPU同步工作的同步对象 围栏
    pID3DDevice->CreateFence(0
        , D3D12_FENCE_FLAG_NONE
        , IID_PPV_ARGS(&pIFence));
    n64FenceValue = 1;//围栏值

    //创建一个Event同步对象，用于等待围栏事件通知
    hFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (hFenceEvent == nullptr)
    {
        HRESULT_FROM_WIN32(GetLastError());
    }

    //创建交换链
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};//声明交换链结构体变量
    //填写交换链结构体
    swapChainDesc.BufferCount = nFrameBackBufCount;
    swapChainDesc.Width = iWidth;
    swapChainDesc.Height = iHeight;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;
    //为窗口句柄创建交换链
    pIDXGIFactory5->CreateSwapChainForHwnd(
        pICommandQueue.Get(),
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &pISwapChain1//此处只能使用低版本交换链
    );

    //得到当前后缓冲区的序号，也就是下一个将要呈送显示的缓冲区的序号
    //As函数是为了使用低版本的SwapChain1接口得到高版本的SwapChain3接口，是智能指针com的一种方法
    //目的是为了调用GetCurrentBackBufferIndex方法
    pISwapChain1.As(&pISwapChain3);
    //注意此处使用了高版本的SwapChain3接口的函数
    nFrameIndex = pISwapChain3->GetCurrentBackBufferIndex();

    //----------------------------------------------------------------------------------
    //创建RTV描述符堆
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = nFrameBackBufCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    pID3DDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&pIRTVHeap));
    //得到每个描述符元素的大小，方便下面遍历时偏移迭代器
    nRTVDescriptorSize = pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    //创建RTV描述符
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pIRTVHeap->GetCPUDescriptorHandleForHeapStart());
    pIARenderTargets = new ComPtr<ID3D12Resource>[nFrameBackBufCount];
    //上面rtvHandle可以理解为迭代器，方便为下面for循环创建RTV
    for (UINT i = 0; i < nFrameBackBufCount; i++)
    {
        pISwapChain3->GetBuffer(i, IID_PPV_ARGS(&pIARenderTargets[i]));
        pID3DDevice->CreateRenderTargetView(pIARenderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, nRTVDescriptorSize);//迭代器offset
    }

    //创建深度缓冲及深度缓冲描述符堆
    D3D12_CLEAR_VALUE stDepthOptimizedClearValue = {};
    stDepthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    stDepthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    stDepthOptimizedClearValue.DepthStencil.Stencil = 0;


    CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC tex2D = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D24_UNORM_S8_UINT, iWidth, iHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    //使用隐式默认堆创建一个深度蜡板缓冲区，
    //因为基本上深度缓冲区会一直被使用，重用的意义不大，所以直接使用隐式堆，图方便
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
    //根签名的创建
    //创建根描述符
    D3D12_FEATURE_DATA_ROOT_SIGNATURE stFeatureData = {};
    // 检测是否支持V1.1版本的根签名
    stFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(pID3DDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &stFeatureData, sizeof(stFeatureData))))
    {
        stFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }
    //动态采样器版本------------------------------------------------------------------------------
    CD3DX12_DESCRIPTOR_RANGE1 stDSPRanges[3] = {};
    stDSPRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE);
    stDSPRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
    stDSPRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

    CD3DX12_ROOT_PARAMETER1 stRootParameters[3] = {};
    stRootParameters[0].InitAsDescriptorTable(1, &stDSPRanges[0], D3D12_SHADER_VISIBILITY_PIXEL);//SRV仅PS可见
    stRootParameters[1].InitAsDescriptorTable(1, &stDSPRanges[1], D3D12_SHADER_VISIBILITY_ALL); //CBV是所有Shader可见
    stRootParameters[2].InitAsDescriptorTable(1, &stDSPRanges[2], D3D12_SHADER_VISIBILITY_PIXEL);//SAMPLE仅PS可见

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
    //使用高版本并向下兼容的方法来编译根签名
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

    //编译Shader
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


    //填充用于描述顶点输入布局的结构体
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

    CD3DX12_RASTERIZER_DESC rasterizerDesc = {};//光栅化器描述结构体
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;//设置填充模式\线框模式
    rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;//设置剔除模式

    //创建渲染管线状态对象接口1
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc1 = {};
    psoDesc1.InputLayout = { inputElementDescs1, _countof(inputElementDescs1) };//_countof宏，方便获取静态数组元素个数
    psoDesc1.pRootSignature = pIRootSignature.Get();
    psoDesc1.VS = CD3DX12_SHADER_BYTECODE(vertexShader1.Get());
    psoDesc1.PS = CD3DX12_SHADER_BYTECODE(pixelShader1.Get());
    psoDesc1.RasterizerState = rasterizerDesc;
    psoDesc1.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc1.DepthStencilState.StencilEnable = FALSE;
    psoDesc1.DepthStencilState.DepthEnable = TRUE;			//打开深度缓冲				
    psoDesc1.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//启用深度缓存写入功能
    psoDesc1.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;     //深度测试函数（该值为普通的深度测试，即较小值写入）
    psoDesc1.SampleMask = UINT_MAX;
    psoDesc1.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc1.NumRenderTargets = 1;
    psoDesc1.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc1.SampleDesc.Count = 1;
    pID3DDevice->CreateGraphicsPipelineState(&psoDesc1, IID_PPV_ARGS(&pIPipelineState1));

    //创建渲染管线状态对象接口2
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc2 = {};
    psoDesc2.InputLayout = { inputElementDescs2, _countof(inputElementDescs2) };//_countof宏，方便获取静态数组元素个数
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

    //声明和填充采样器堆结构体
    D3D12_DESCRIPTOR_HEAP_DESC stSamplerHeapDesc = {};
    stSamplerHeapDesc.NumDescriptors = nSampleMaxCnt;
    stSamplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    stSamplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    //创建采样器堆
    pID3DDevice->CreateDescriptorHeap(
        &stSamplerHeapDesc,
        IID_PPV_ARGS(&pISamplerDescriptorHeap));
    //获取采样器堆的大小，方便后续利用offset的形式创建多个采样器
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

    //填充每帧渲染开始和结束两个资源屏障结构体
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