#include "stdafx.h"

using namespace Microsoft;
using namespace Microsoft::WRL;
using namespace DirectX;


//窗体备用
HWND g_hwnd;
HINSTANCE g_hInstance;


struct GRS_VERTEX
{
    XMFLOAT3 position;
    XMFLOAT4 color;
};

//过程函数
LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY://销毁窗体
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{

    const UINT nFrameBackBufCount = 3u;
    const float	faClearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    int iWidth = 1024;
    int iHeight = 768;
    UINT nFrameIndex = 0;
    UINT nFrame = 0;

    UINT nDXGIFactoryFlags = 0U;
    UINT nRTVDescriptorSize = 0U;

    float fAspectRatio = 3.0f;

    MSG msg;

    D3D12_VERTEX_BUFFER_VIEW stVertexBufferView = {};

    UINT64 n64FenceValue = 0ui64;
    HANDLE hFenceEvent = nullptr;

    CD3DX12_VIEWPORT stViewPort(0.0f, 0.0f, static_cast<float>(iWidth), static_cast<float>(iHeight));
    CD3DX12_RECT  stScissorRect(0, 0, static_cast<LONG>(iWidth), static_cast<LONG>(iHeight));

    ComPtr<IDXGIFactory5>                pIDXGIFactory5;
    ComPtr<IDXGIAdapter1>                pIAdapter;
    ComPtr<ID3D12Device4>                pID3DDevice;
    ComPtr<ID3D12CommandQueue>           pICommandQueue;
    ComPtr<IDXGISwapChain1>              pISwapChain1;
    ComPtr<IDXGISwapChain3>              pISwapChain3;
    ComPtr<ID3D12DescriptorHeap>         pIRTVHeap;
    ComPtr<ID3D12Resource>               pIARenderTargets[nFrameBackBufCount];
    ComPtr<ID3D12CommandAllocator>       pICommandAllocator;
    ComPtr<ID3D12RootSignature>          pIRootSignature;
    ComPtr<ID3D12PipelineState>          pIPipelineState;
    ComPtr<ID3D12GraphicsCommandList>    pICommandList;
    ComPtr<ID3D12Resource>               pIVertexBuffer;
    ComPtr<ID3D12Fence>                  pIFence;
    

    //填充窗口类
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);//指定结构体的大小，直接使用 sizeof(WNDCLASSEX) 赋值
    wcex.style = CS_GLOBALCLASS;//窗口样式
    wcex.lpfnWndProc = WinProc;//窗口过程函数 指针
    wcex.cbClsExtra = 0;//扩展数据大小，一般填充0
    wcex.cbWndExtra = 0;//扩展数据大小，一般填充0
    wcex.hInstance = hInstance;//应用程序的实例句柄。该值就是 WinMain 函数 的 hInstance 参数
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);//窗口类的图标句柄，这里使用默认的应用程序图标
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);//窗口类的光标句柄，这里使用默认的箭头图标
    wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);//窗口类背景颜色的画刷句柄，防止系统出发无聊的背景重绘
    wcex.lpszMenuName = NULL;//窗口类的菜单句柄，这里没有菜单，填 NULL
    wcex.lpszClassName = TEXT("WinApp");;//字符串，用来标识一个窗口类，进程内必须唯一，不可以重名
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);//窗口类的小图标句柄，这里和 hIcon 指定相同的图标


    //注册窗口类，函数接收上述填充的窗口类的指针
    if (!RegisterClassEx(&wcex))
    {
        return 0;
    }
    g_hInstance = hInstance;


    //创建窗口
    HWND hwnd;

    if (!(hwnd = CreateWindowEx(
        0, //窗口扩展样式（例如让窗口透明），一般直接传空
        TEXT("WinApp"), // 填充窗口类 的时候所起的名字，用来建立窗体和窗口类的关联
        TEXT("HelloWord"), //窗口的标题名
        WS_OVERLAPPED | WS_SYSMENU,//窗口的样式
        0, //X轴位置
        0, //Y轴位置
        iWidth, //窗口宽度
        iHeight, //窗口高度
        NULL, //是否有父窗体
        NULL, //是否窗体有菜单
        hInstance, //窗体应用实例句柄，传 WinMain 函数 的第一个参数
        NULL//附加参数
        )))
    {
        return 0;
    }
    //显示更新窗口
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    g_hwnd = hwnd;


    //创建factory，以便为后续部分创建适配器，设备等
    CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(&pIDXGIFactory5));
    //关闭ALT+ENTER键切换全屏的功能，因为没有实现OnSize处理，所以先关闭
    pIDXGIFactory5->MakeWindowAssociation(g_hwnd, DXGI_MWA_NO_ALT_ENTER);


    //枚举并选择一个合适的显示适配器（显卡）来创建D3D设备接口
    for (UINT adapterIndex = 0
        ; DXGI_ERROR_NOT_FOUND != pIDXGIFactory5->EnumAdapters1(adapterIndex, &pIAdapter)
        ; ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc = {};
        pIAdapter->GetDesc1(&desc);
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)//可在此处打断点，查看选择了哪张显卡作为适配器创建设备
        {//软件虚拟适配器，跳过
            continue;
        }
        //检查适配器对D3D支持的兼容级别，这里直接要求支持12.1的能力，注意返回接口的那个参数被置为了nullptr，这样
        //就不会实际创建一个设备了，也不用我们啰嗦的再调用release来释放接口。这也是一个重要的技巧，请记住！
        if (SUCCEEDED(D3D12CreateDevice(pIAdapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr)))
        {
            break;
        }
    }
    //创建D3D12.1的设备
    D3D12CreateDevice(pIAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&pID3DDevice));
    
    
    //创建D3D2命令队列接口
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    pID3DDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pICommandQueue));
    

    //创建交换链
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = nFrameBackBufCount;
    swapChainDesc.Width = iWidth;
    swapChainDesc.Height = iHeight;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    pIDXGIFactory5->CreateSwapChainForHwnd(
        pICommandQueue.Get(),
        g_hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &pISwapChain1
    );
    
    //得到当前后缓冲区的序号，也就是下一个将要呈送显示的缓冲区的序号
    pISwapChain1.As(&pISwapChain3);
    //注意此处使用了高版本的SwapChain接口的函数
    nFrameIndex = pISwapChain3->GetCurrentBackBufferIndex();


    //创建RTV描述符堆
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = nFrameBackBufCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    pID3DDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&pIRTVHeap));
    //得到每个描述符元素的大小
    nRTVDescriptorSize = pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    
    //创建RTV描述符
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pIRTVHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < nFrameBackBufCount; i++)
    {
        pISwapChain3->GetBuffer(i, IID_PPV_ARGS(&pIARenderTargets[i]));
        pID3DDevice->CreateRenderTargetView(pIARenderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, nRTVDescriptorSize);
    }


    //创建根签名
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;

    D3D12SerializeRootSignature(&rootSignatureDesc
        , D3D_ROOT_SIGNATURE_VERSION_1
        , &signature, &error);

    pID3DDevice->CreateRootSignature(0
        , signature->GetBufferPointer()
        , signature->GetBufferSize()
        , IID_PPV_ARGS(&pIRootSignature));


    //创建命令列表分配器
    pID3DDevice->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT
        , IID_PPV_ARGS(&pICommandAllocator));
    //创建图形命令列表
    pID3DDevice->CreateCommandList(0
        , D3D12_COMMAND_LIST_TYPE_DIRECT
        , pICommandAllocator.Get()
        , pIPipelineState.Get()
        , IID_PPV_ARGS(&pICommandList));


    //编译Shader及创建渲染管线状态对象接口
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    TCHAR pszShaderFileName[] = _T("D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Shaders\\shaders.hlsl");

    D3DCompileFromFile(pszShaderFileName, nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);

    D3DCompileFromFile(pszShaderFileName, nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);

    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }

    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = pIRootSignature.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    pID3DDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pIPipelineState));


    // 定义三角形的3D数据结构，每个顶点使用三原色之一
    GRS_VERTEX triangleVertices[] =
    {
        { { 0.0f, 0.25f * fAspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.25f * fAspectRatio, -0.25f * fAspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -0.25f * fAspectRatio, -0.25f * fAspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
    };

    const UINT vertexBufferSize = sizeof(triangleVertices);

    D3D12_HEAP_PROPERTIES stHeapProp = { D3D12_HEAP_TYPE_UPLOAD };

    D3D12_RESOURCE_DESC stResSesc = {};
    stResSesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    stResSesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    stResSesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    stResSesc.Format = DXGI_FORMAT_UNKNOWN;
    stResSesc.Width = vertexBufferSize;
    stResSesc.Height = 1;
    stResSesc.DepthOrArraySize = 1;
    stResSesc.MipLevels = 1;
    stResSesc.SampleDesc.Count = 1;
    stResSesc.SampleDesc.Quality = 0;


    pID3DDevice->CreateCommittedResource(
        &stHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &stResSesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pIVertexBuffer));

    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    pIVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
    pIVertexBuffer->Unmap(0, nullptr);

    stVertexBufferView.BufferLocation = pIVertexBuffer->GetGPUVirtualAddress();
    stVertexBufferView.StrideInBytes = sizeof(GRS_VERTEX);
    stVertexBufferView.SizeInBytes = vertexBufferSize;


    //创建一个同步对象 围栏，用于等待渲染完成，因为现在Draw Call是异步的了
    pID3DDevice->CreateFence(0
        , D3D12_FENCE_FLAG_NONE
        , IID_PPV_ARGS(&pIFence));
    n64FenceValue = 1;
    //创建一个Event同步对象，用于等待围栏事件通知
    hFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (hFenceEvent == nullptr)
    {
        HRESULT_FROM_WIN32(GetLastError());
    }



    // 填充资源屏障结构
    D3D12_RESOURCE_BARRIER stBeginResBarrier = {};
    stBeginResBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    stBeginResBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    stBeginResBarrier.Transition.pResource = pIARenderTargets[nFrameIndex].Get();
    stBeginResBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    stBeginResBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    stBeginResBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    D3D12_RESOURCE_BARRIER stEndResBarrier = {};
    stEndResBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    stEndResBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    stEndResBarrier.Transition.pResource = pIARenderTargets[nFrameIndex].Get();
    stEndResBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    stEndResBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    stEndResBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    D3D12_CPU_DESCRIPTOR_HANDLE stRTVHandle = pIRTVHeap->GetCPUDescriptorHandleForHeapStart();
    DWORD dwRet = 0;
    BOOL bExit = FALSE;

    pICommandList->Close();
    SetEvent(hFenceEvent);
    
    while (!bExit)
    {
        dwRet = ::MsgWaitForMultipleObjects(1, &hFenceEvent, FALSE, INFINITE, QS_ALLINPUT);
        switch (dwRet - WAIT_OBJECT_0)
        {
        case 0:
        {
            //获取新的后缓冲序号，因为Present真正完成时后缓冲的序号就更新了
            nFrameIndex = pISwapChain3->GetCurrentBackBufferIndex();

            //命令分配器先Reset一下
            pICommandAllocator->Reset();
            //Reset命令列表，并重新指定命令分配器和PSO对象
            pICommandList->Reset(pICommandAllocator.Get(), pIPipelineState.Get());

            //开始记录命令
            pICommandList->SetGraphicsRootSignature(pIRootSignature.Get());
            pICommandList->SetPipelineState(pIPipelineState.Get());
            pICommandList->RSSetViewports(1, &stViewPort);
            pICommandList->RSSetScissorRects(1, &stScissorRect);

            // 通过资源屏障判定后缓冲已经切换完毕可以开始渲染了
            stBeginResBarrier.Transition.pResource = pIARenderTargets[nFrameIndex].Get();
            pICommandList->ResourceBarrier(1, &stBeginResBarrier);

            stRTVHandle = pIRTVHeap->GetCPUDescriptorHandleForHeapStart();
            stRTVHandle.ptr += nFrameIndex * nRTVDescriptorSize;
            //设置渲染目标
            pICommandList->OMSetRenderTargets(1, &stRTVHandle, FALSE, nullptr);

            // 继续记录命令，并真正开始新一帧的渲染

            pICommandList->ClearRenderTargetView(stRTVHandle, faClearColor, 0, nullptr);
            pICommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            pICommandList->IASetVertexBuffers(0, 1, &stVertexBufferView);

            //Draw Call！！！
            pICommandList->DrawInstanced(3, 1, 0, 0);

            //又一个资源屏障，用于确定渲染已经结束可以提交画面去显示了
            stEndResBarrier.Transition.pResource = pIARenderTargets[nFrameIndex].Get();
            pICommandList->ResourceBarrier(1, &stEndResBarrier);
            //关闭命令列表，可以去执行了
            pICommandList->Close();

            //执行命令列表
            ID3D12CommandList* ppCommandLists[] = { pICommandList.Get() };
            pICommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

            //提交画面
            pISwapChain3->Present(1, 0);

            //开始同步GPU与CPU的执行，先记录围栏标记值
            const UINT64 n64CurrentFenceValue = n64FenceValue;
            pICommandQueue->Signal(pIFence.Get(), n64CurrentFenceValue);
            n64FenceValue++;
            pIFence->SetEventOnCompletion(n64CurrentFenceValue, hFenceEvent);
        }
        break;
        case 1:
        {//处理消息
            while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if (WM_QUIT != msg.message)
                {
                    ::TranslateMessage(&msg);
                    ::DispatchMessage(&msg);
                }
                else
                {
                    bExit = TRUE;
                }
            }
        }
        break;
        case WAIT_TIMEOUT:
        {

        }
        break;
        default:
            break;
        }
    }
    return 0;
}