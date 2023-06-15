#include "stdafx.h"

using namespace Microsoft;
using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;


//���屸��
HWND g_hwnd;
HINSTANCE g_hInstance;

//���嶥��ṹ��
struct GRS_VERTEX
{
    XMFLOAT3 position;//�ȼ���float[3]
    XMFLOAT4 color;//�ȼ���float[4]
};

//���̺���
LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY://���ٴ���
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    //ÿ֡��������������˫����Ϊ2��������Ϊ3�������u�Ǻ�׺�������ֽ���Ϊ�޷�������
    const UINT nFrameBackBufCount = 3u;
    //ÿ֡����������������ɼ�ÿ֡����������Ϊ3���������������Ϊ0��1��2
    UINT nFrameIndex = 0;

    
    int iWidth = 1024;//���ڿ��
    int iHeight = 768;//���ڸ߶�
    
    UINT nFrame = 0;
    
    //0uָ���ǣ�������0����Ϊ�޷����������ͣ�uΪ��׺
    UINT nDXGIFactoryFlags = 0u;//���ڴ���DX������flag����Ϊ����������Ҫ�ṩһ��flag
    
    UINT nRTVDescriptorSize = 0u;//���������Ĵ�С

    float fAspectRatio = 3.0f;//���ڿ��ƻ��������δ�С�ı���ֵ

    MSG msg;//���ڵ���Ϣ

    //һ�������ṩ������������еĶ���buffer��view�������˶���buffer����Ϣ
    D3D12_VERTEX_BUFFER_VIEW stVertexBufferView = {};
    
    //Χ���ı��ֵ��0Ϊ���֣�ui64Ϊ��׺����ʾ��0ָ����ֵΪ�޷��� 64 λ��������
    UINT64 n64FenceValue = 0ui64;

    //���ڴ�Ŵ�����event�����ָ��
    HANDLE hFenceEvent = nullptr;

    //�����ṩ������������е�DX�ӿڣ��˴�ֱ�Ӹ�ֵΪ�����Ͻ�λ�ã�0��0�������ڿ�͸�
    CD3DX12_VIEWPORT stViewPort(0.0f, 0.0f, static_cast<float>(iWidth), static_cast<float>(iHeight));
    
    //�����ṩ������������еĲ��о��Σ��������ƿɼ����򣬴˴���С��λ�����ӿ�һ��
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
    

    //��䴰����
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);//ָ���ṹ��Ĵ�С��ֱ��ʹ�� sizeof(WNDCLASSEX) ��ֵ
    wcex.style = CS_GLOBALCLASS;//������ʽ
    wcex.lpfnWndProc = WinProc;//���ڹ��̺��� ָ��
    wcex.cbClsExtra = 0;//��չ���ݴ�С��һ�����0
    wcex.cbWndExtra = 0;//��չ���ݴ�С��һ�����0
    wcex.hInstance = hInstance;//Ӧ�ó����ʵ���������ֵ���� WinMain ���� �� hInstance ����
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);//�������ͼ����������ʹ��Ĭ�ϵ�Ӧ�ó���ͼ��
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);//������Ĺ����������ʹ��Ĭ�ϵļ�ͷͼ��
    wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);//�����౳����ɫ�Ļ�ˢ�������ֹϵͳ�������ĵı����ػ�
    wcex.lpszMenuName = NULL;//������Ĳ˵����������û�в˵����� NULL
    wcex.lpszClassName = TEXT("WinApp");;//�ַ�����������ʶһ�������࣬�����ڱ���Ψһ������������
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);//�������Сͼ����������� hIcon ָ����ͬ��ͼ��


    //ע�ᴰ���࣬���������������Ĵ������ָ��
    if (!RegisterClassEx(&wcex))
    {
        return 0;
    }
    g_hInstance = hInstance;


    //��������
    HWND hwnd;

    if (!(hwnd = CreateWindowEx(
        0, //������չ��ʽ�������ô���͸������һ��ֱ�Ӵ���
        TEXT("WinApp"), // ��䴰���� ��ʱ����������֣�������������ʹ�����Ĺ���
        TEXT("HelloWord"), //���ڵı�����
        WS_OVERLAPPED | WS_SYSMENU,//���ڵ���ʽ
        0, //X��λ��
        0, //Y��λ��
        iWidth, //���ڿ��
        iHeight, //���ڸ߶�
        NULL, //�Ƿ��и�����
        NULL, //�Ƿ����в˵�
        hInstance, //����Ӧ��ʵ��������� WinMain ���� �ĵ�һ������
        NULL//���Ӳ���
        )))
    {
        return 0;
    }
    //��ʾ���´���
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    g_hwnd = hwnd;



    //����factory���Ա�Ϊ�������ִ������������豸��
    CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(&pIDXGIFactory5));
    
    //�ر�ALT+ENTER���л�ȫ���Ĺ��ܣ���Ϊû��ʵ��OnSize���������ȹر�
    pIDXGIFactory5->MakeWindowAssociation(g_hwnd, DXGI_MWA_NO_ALT_ENTER);



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
    

    
    //����DX�������
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    pID3DDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pICommandQueue));
    


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
        g_hwnd,
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
    //����rtvHandle�������Ϊ������������Ϊ����forѭ������RTV
    for (UINT i = 0; i < nFrameBackBufCount; i++)
    {
        pISwapChain3->GetBuffer(i, IID_PPV_ARGS(&pIARenderTargets[i]));
        pID3DDevice->CreateRenderTargetView(pIARenderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, nRTVDescriptorSize);//������offset
    }



    //������ǩ��������������������Ⱦ����Դ�Ĵ洢����
    //������ͳһ���й����ɢ�ڸ�����Դ�������������еĴ洢Slot�Ͷ�Ӧ�Ĵ�����ŵĶ���
    //�����ڴ���ĳ����Դʱ�������������ָ����Ӧ�ĸ�Slot�ͼĴ����������
    //����ͳһ��D3D12����һ����ǩ���ͽ���Щ�������
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
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



    //����Shader��������Ⱦ����״̬����ӿ�
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

    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };//_countof�꣬�����ȡ��̬����Ԫ�ظ���
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



    // ���������ε�3D���ݽṹ��ÿ������ʹ����ԭɫ֮һ
    GRS_VERTEX triangleVertices[] =
    {
        { { 0.0f, 0.25f * fAspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.25f * fAspectRatio, -0.25f * fAspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -0.25f * fAspectRatio, -0.25f * fAspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
    };

    //�����Դ�ṹ�弰�����ύ��Դ����
    const UINT vertexBufferSize = sizeof(triangleVertices);
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

    D3D12_HEAP_PROPERTIES stHeapProp = { D3D12_HEAP_TYPE_UPLOAD };//Upload�ĸ�����Ǵ�CPU�ڴ��ϴ����Դ��е���˼
    pID3DDevice->CreateCommittedResource(
        &stHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &stResSesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pIVertexBuffer));
    //�������ļ�Ϊ�õ�Ҫ���ƵĶ���buffer���������潫�������ݴ�CPU�ڴ����Դ���д���
    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    pIVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
    pIVertexBuffer->Unmap(0, nullptr);

    //��䶥��buffer��ͼ�Ľṹ�壬����GPU����������Դʵ����Vertex Buffer
    stVertexBufferView.BufferLocation = pIVertexBuffer->GetGPUVirtualAddress();
    stVertexBufferView.StrideInBytes = sizeof(GRS_VERTEX);
    stVertexBufferView.SizeInBytes = vertexBufferSize;



    //���������б������
    pID3DDevice->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT
        , IID_PPV_ARGS(&pICommandAllocator));
    //����ͼ�������б�
    pID3DDevice->CreateCommandList(0
        , D3D12_COMMAND_LIST_TYPE_DIRECT
        , pICommandAllocator.Get()
        , pIPipelineState.Get()
        , IID_PPV_ARGS(&pICommandList));



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



    // ��俪ʼ�ͽ���������Դ���Ͻṹ��
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



    //������ʱ�������Ա��ڴ�����Ч����Ϣѭ��
    HANDLE phWait = CreateWaitableTimer(NULL, FALSE, NULL);
    LARGE_INTEGER liDueTime = {};

    liDueTime.QuadPart = -1i64;//1���ʼ��ʱ
    if(phWait)
        SetWaitableTimer(phWait, &liDueTime, 1000, NULL, NULL, 0);//���ڴ�����ʱ�����ڣ���λΪ����
        //�˴�1000����ָ����ÿ��һ�����һ����Ϣѭ��
    //��ʼ��Ϣѭ�����������в�����Ⱦ
    DWORD dwRet = 0;
    BOOL bExit = FALSE;
    while (!bExit)
    {
        dwRet = ::MsgWaitForMultipleObjects(1, &phWait, FALSE, INFINITE, QS_ALLINPUT);
        switch (dwRet - WAIT_OBJECT_0)
        {
        case 0:
        case WAIT_TIMEOUT:
        {//��ʱ��ʱ�䵽
             //��ʼ��¼����
            pICommandList->SetGraphicsRootSignature(pIRootSignature.Get());
            pICommandList->RSSetViewports(1, &stViewPort);
            pICommandList->RSSetScissorRects(1, &stScissorRect);


            // ͨ����Դ�����ж��󻺳��Ѿ��л���Ͽ��Կ�ʼ��Ⱦ��
            stBeginResBarrier.Transition.pResource = pIARenderTargets[nFrameIndex].Get();
            pICommandList->ResourceBarrier(1, &stBeginResBarrier);

            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pIRTVHeap->GetCPUDescriptorHandleForHeapStart()
                , nFrameIndex
                , nRTVDescriptorSize);

            //������ȾĿ��
            pICommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
            // ������¼�����������ʼ��һ֡����Ⱦ
            const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
            pICommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
            pICommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            pICommandList->IASetVertexBuffers(0, 1, &stVertexBufferView);
            //Draw Call
            pICommandList->DrawInstanced(3, 1, 0, 0);
            //��һ����Դ���ϣ�����ȷ����Ⱦ�Ѿ����������ύ����ȥ��ʾ��
            stEndResBarrier.Transition.pResource = pIARenderTargets[nFrameIndex].Get();
            pICommandList->ResourceBarrier(1, &stEndResBarrier);
            //�ر������б�����ȥִ����
            pICommandList->Close();
            //ִ�������б�
            ID3D12CommandList* ppCommandLists[] = { pICommandList.Get() };
            pICommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

            //�ύ����
            pISwapChain3->Present(1, 0);
            //��ʼͬ��GPU��CPU��ִ�У��ȼ�¼Χ�����ֵ
            const UINT64 fence = n64FenceValue;
            pICommandQueue->Signal(pIFence.Get(), fence);
            n64FenceValue++;

            // ��������û������ִ�е�Χ����ǵ����û�о������¼�ȥ�ȴ���ע��ʹ�õ���������ж����ָ��
            if (pIFence->GetCompletedValue() < fence)
            {
                pIFence->SetEventOnCompletion(fence, hFenceEvent);
                WaitForSingleObject(hFenceEvent, INFINITE);
            }

            //������˵��һ���������������ִ�����ˣ�������ʹ������ǵ�һ֡�Ѿ���Ⱦ���ˣ�����׼��ִ����һ֡//��Ⱦ
            //��ȡ�µĺ󻺳���ţ���ΪPresent�������ʱ�󻺳����ž͸�����
            nFrameIndex = pISwapChain3->GetCurrentBackBufferIndex();
            //�����������Resetһ��
            pICommandAllocator->Reset();
            //Reset�����б�������ָ�������������PSO����
            pICommandList->Reset(pICommandAllocator.Get(), pIPipelineState.Get());
            
            
            nFrame++;
            wstring str1 = L"��";
            wstring str2 = to_wstring(nFrame);
            wstring str3 = L"֡�������\n";
            wstring result = str1 + str2 + str3;
            LPCWSTR frameInfo = result.c_str();
            OutputDebugString(frameInfo);
            
        }
        break;
        case 1:
        {//������Ϣ
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
        default:
            break;
        }

    }



    return 0;
}