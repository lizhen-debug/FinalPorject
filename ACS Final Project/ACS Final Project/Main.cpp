#include "stdafx.h"

//#define Render_Base_Triangle
#define Render_Base_Rectangle_With_Texture


//�¶���ĺ�������ȡ������
#define GRS_UPPER_DIV(A,B) ((UINT)(((A)+((B)-1))/(B)))

//���������ϱ߽�����㷨 �ڴ�����г��� ���ס
#define GRS_UPPER(A,B) ((UINT)(((A)+((B)-1))&~(B - 1)))


using namespace Microsoft;
using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;

#ifdef Render_Base_Rectangle_With_Texture

//DX12ʾ�����е��������ڸ�ʽƥ��Ĺ��ߺ���
struct WICTranslate
{
    GUID wic;
    DXGI_FORMAT format;
};

static WICTranslate g_WICFormats[] =
{//WIC��ʽ��DXGI���ظ�ʽ�Ķ�Ӧ���ñ��еĸ�ʽΪ��֧�ֵĸ�ʽ
    { GUID_WICPixelFormat128bppRGBAFloat,       DXGI_FORMAT_R32G32B32A32_FLOAT },

    { GUID_WICPixelFormat64bppRGBAHalf,         DXGI_FORMAT_R16G16B16A16_FLOAT },
    { GUID_WICPixelFormat64bppRGBA,             DXGI_FORMAT_R16G16B16A16_UNORM },

    { GUID_WICPixelFormat32bppRGBA,             DXGI_FORMAT_R8G8B8A8_UNORM },
    { GUID_WICPixelFormat32bppBGRA,             DXGI_FORMAT_B8G8R8A8_UNORM }, // DXGI 1.1
    { GUID_WICPixelFormat32bppBGR,              DXGI_FORMAT_B8G8R8X8_UNORM }, // DXGI 1.1

    { GUID_WICPixelFormat32bppRGBA1010102XR,    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM }, // DXGI 1.1
    { GUID_WICPixelFormat32bppRGBA1010102,      DXGI_FORMAT_R10G10B10A2_UNORM },

    { GUID_WICPixelFormat16bppBGRA5551,         DXGI_FORMAT_B5G5R5A1_UNORM },
    { GUID_WICPixelFormat16bppBGR565,           DXGI_FORMAT_B5G6R5_UNORM },

    { GUID_WICPixelFormat32bppGrayFloat,        DXGI_FORMAT_R32_FLOAT },
    { GUID_WICPixelFormat16bppGrayHalf,         DXGI_FORMAT_R16_FLOAT },
    { GUID_WICPixelFormat16bppGray,             DXGI_FORMAT_R16_UNORM },
    { GUID_WICPixelFormat8bppGray,              DXGI_FORMAT_R8_UNORM },

    { GUID_WICPixelFormat8bppAlpha,             DXGI_FORMAT_A8_UNORM },
};

// WIC ���ظ�ʽת����.
struct WICConvert
{
    GUID source;
    GUID target;
};

static WICConvert g_WICConvert[] =
{
    // Ŀ���ʽһ������ӽ��ı�֧�ֵĸ�ʽ
    { GUID_WICPixelFormatBlackWhite,            GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM

    { GUID_WICPixelFormat1bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat2bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat4bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat8bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM

    { GUID_WICPixelFormat2bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM
    { GUID_WICPixelFormat4bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM

    { GUID_WICPixelFormat16bppGrayFixedPoint,   GUID_WICPixelFormat16bppGrayHalf }, // DXGI_FORMAT_R16_FLOAT
    { GUID_WICPixelFormat32bppGrayFixedPoint,   GUID_WICPixelFormat32bppGrayFloat }, // DXGI_FORMAT_R32_FLOAT

    { GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat16bppBGRA5551 }, // DXGI_FORMAT_B5G5R5A1_UNORM

    { GUID_WICPixelFormat32bppBGR101010,        GUID_WICPixelFormat32bppRGBA1010102 }, // DXGI_FORMAT_R10G10B10A2_UNORM

    { GUID_WICPixelFormat24bppBGR,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat24bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat32bppPBGRA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat32bppPRGBA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM

    { GUID_WICPixelFormat48bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat48bppBGR,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppBGRA,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppPRGBA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppPBGRA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

    { GUID_WICPixelFormat48bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
    { GUID_WICPixelFormat48bppBGRFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
    { GUID_WICPixelFormat64bppRGBAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
    { GUID_WICPixelFormat64bppBGRAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
    { GUID_WICPixelFormat64bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
    { GUID_WICPixelFormat48bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
    { GUID_WICPixelFormat64bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT

    { GUID_WICPixelFormat128bppPRGBAFloat,      GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT
    { GUID_WICPixelFormat128bppRGBFloat,        GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT
    { GUID_WICPixelFormat128bppRGBAFixedPoint,  GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT
    { GUID_WICPixelFormat128bppRGBFixedPoint,   GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT
    { GUID_WICPixelFormat32bppRGBE,             GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT

    { GUID_WICPixelFormat32bppCMYK,             GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat64bppCMYK,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat40bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat80bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

    { GUID_WICPixelFormat32bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat64bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppPRGBAHalf,        GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT
};

bool GetTargetPixelFormat(const GUID* pSourceFormat, GUID* pTargetFormat)
{//���ȷ�����ݵ���ӽ���ʽ���ĸ�
    *pTargetFormat = *pSourceFormat;
    for (size_t i = 0; i < _countof(g_WICConvert); ++i)
    {
        if (InlineIsEqualGUID(g_WICConvert[i].source, *pSourceFormat))
        {
            *pTargetFormat = g_WICConvert[i].target;
            return true;
        }
    }
    return false;
}

DXGI_FORMAT GetDXGIFormatFromPixelFormat(const GUID* pPixelFormat)
{//���ȷ�����ն�Ӧ��DXGI��ʽ����һ��
    for (size_t i = 0; i < _countof(g_WICFormats); ++i)
    {
        if (InlineIsEqualGUID(g_WICFormats[i].wic, *pPixelFormat))
        {
            return g_WICFormats[i].format;
        }
    }
    return DXGI_FORMAT_UNKNOWN;
}

struct GRS_VERTEX
{
    XMFLOAT3 m_vPos;		//Position
    XMFLOAT2 m_vTxc;		//Texcoord
};

#endif // Render_Base_Rectangle_With_Texture

//���屸��
HWND g_hwnd;
HINSTANCE g_hInstance;

#ifdef Render_Base_Triangle

//���嶥��ṹ��
struct GRS_VERTEX
{
    XMFLOAT3 position;//�ȼ���float[3]
    XMFLOAT4 color;//�ȼ���float[4]
};

#endif // Render_Base_Triangle

UINT nCurrentSamplerNO = 0; //��ǰʹ�õĲ���������
UINT nSampleMaxCnt = 5;		//����������͵Ĳ�����

//���̺���
LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY://���ٴ���
        PostQuitMessage(0);
        break;

    case WM_KEYUP:
    {
        if (VK_SPACE == (wParam & 0xFF))
        {//���ո���л���ͬ�Ĳ�������Ч����������ÿ�ֲ���������ĺ���
            //UINT nCurrentSamplerNO = 0; //��ǰʹ�õĲ���������
            //UINT nSampleMaxCnt = 5;		//����������͵Ĳ�����
            ++nCurrentSamplerNO;
            nCurrentSamplerNO %= nSampleMaxCnt;
        }
    }
    break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    //ʹ��com����ָ��ĳ�ʼ���������õĻ��п��ܳ��ֳ����˳�ʱָ���ͷŲ����ڴ���ʳ�ͻ���쳣
    HRESULT hr = CoInitialize(nullptr);

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
    //����DX����
    ComPtr<IDXGIFactory5>                pIDXGIFactory5;
    ComPtr<IDXGIAdapter1>                pIAdapter;

    ComPtr<ID3D12Device4>                pID3DDevice;
    ComPtr<ID3D12CommandQueue>           pICommandQueue;
    ComPtr<ID3D12CommandAllocator>       pICommandAllocator;
    ComPtr<ID3D12GraphicsCommandList>    pICommandList;

    ComPtr<IDXGISwapChain1>              pISwapChain1;
    ComPtr<IDXGISwapChain3>              pISwapChain3;
    ComPtr<ID3D12Resource>               pIARenderTargets[nFrameBackBufCount];
    ComPtr<ID3D12DescriptorHeap>         pIRTVHeap;

    ComPtr<ID3D12Heap>					 pITextureHeap;
    ComPtr<ID3D12Heap>					 pIUploadHeap;
    ComPtr<ID3D12Resource>				 pITexture;
    ComPtr<ID3D12Resource>				 pITextureUpload;
    ComPtr<ID3D12Resource>				 pIVertexBuffer;
    ComPtr<ID3D12DescriptorHeap>		 pISRVHeap;
    ComPtr<ID3D12DescriptorHeap>		 pISamplerDescriptorHeap;

    ComPtr<ID3D12RootSignature>          pIRootSignature;
    ComPtr<ID3D12PipelineState>          pIPipelineState;
    ComPtr<ID3DBlob>					 pIBlobVertexShader;
    ComPtr<ID3DBlob>					 pIBlobPixelShader;
    ComPtr<ID3D12Fence>                  pIFence;

    //����WIC����
    ComPtr<IWICImagingFactory>			 pIWICFactory;
    ComPtr<IWICBitmapDecoder>			 pIWICDecoder;
    ComPtr<IWICBitmapFrameDecode>		 pIWICFrame;
    ComPtr<IWICBitmapSource>			 pIBMP;

    UINT nTextureW = 0u;//������
    UINT nTextureH = 0u;//����߶�
    UINT nBPP = 0u;//����ÿ��������ռ��λ��
    DXGI_FORMAT stTextureFormat = DXGI_FORMAT_UNKNOWN;//�����ʽ
    UINT nPicRowPitch = 0;
    UINT64 n64UploadBufferSize = 0;

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT stTxtLayouts = {};
    D3D12_RESOURCE_DESC stTextureDesc = {};
    D3D12_RESOURCE_DESC stDestDesc = {};

    UINT nSamplerDescriptorSize = 0; //��������С

    //----------------------------------------------------------------------------------

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

#ifdef Render_Base_Rectangle_With_Texture

    //----------------------------------------------------------------------------------

    //ʹ�ô�COM��ʽ����WIC�೧����Ҳ�ǵ���WIC��һ��Ҫ��������
    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pIWICFactory));

    //ʹ��WIC�೧����ӿڼ�������ͼƬ�����õ�һ��WIC����������ӿڣ�ͼƬ��Ϣ��������ӿڴ���Ķ�������
    TCHAR pszTexcuteFileName[] = _T("D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Textures\\test1.jpg");

    pIWICFactory->CreateDecoderFromFilename(
        pszTexcuteFileName,              // �ļ���
        NULL,                            // ��ָ����������ʹ��Ĭ��
        GENERIC_READ,                    // ����Ȩ��
        WICDecodeMetadataCacheOnDemand,  // ����Ҫ�ͻ������� 
        &pIWICDecoder                    // ����������
    );

    //��ȡ��һ֡ͼƬ����������ͨ����λͼ
    pIWICDecoder->GetFrame(0, &pIWICFrame);

    WICPixelFormatGUID wpf = {};
    //��ȡWICͼƬ��ʽ
    pIWICFrame->GetPixelFormat(&wpf);
    GUID tgFormat = {};

    //ͨ����һ��ת��֮���ȡDXGI�ĵȼ۸�ʽ
    if (GetTargetPixelFormat(&wpf, &tgFormat))
    {
        stTextureFormat = GetDXGIFormatFromPixelFormat(&tgFormat);
    }

    if (DXGI_FORMAT_UNKNOWN == stTextureFormat)
    {// ��֧�ֵ�ͼƬ��ʽ Ŀǰ�˳����� 
     // һ�� ��ʵ�ʵ����浱�ж����ṩ�����ʽת�����ߣ�
     // ͼƬ����Ҫ��ǰת���ã����Բ�����ֲ�֧�ֵ�����
        return 0;
    }

    // ����һ��λͼ��ʽ��ͼƬ���ݶ���ӿ�

    if (!InlineIsEqualGUID(wpf, tgFormat))
    {// ����жϺ���Ҫ�����ԭWIC��ʽ����ֱ����ת��ΪDXGI��ʽ��ͼƬʱ
     // ������Ҫ���ľ���ת��ͼƬ��ʽΪ�ܹ�ֱ�Ӷ�ӦDXGI��ʽ����ʽ
        //����ͼƬ��ʽת����
        ComPtr<IWICFormatConverter> pIConverter;
        pIWICFactory->CreateFormatConverter(&pIConverter);

        //��ʼ��һ��ͼƬת������ʵ��Ҳ���ǽ�ͼƬ���ݽ����˸�ʽת��
        pIConverter->Initialize(
            pIWICFrame.Get(),                // ����ԭͼƬ����
            tgFormat,						 // ָ����ת����Ŀ���ʽ
            WICBitmapDitherTypeNone,         // ָ��λͼ�Ƿ��е�ɫ�壬�ִ��������λͼ�����õ�ɫ�壬����ΪNone
            NULL,                            // ָ����ɫ��ָ��
            0.f,                             // ָ��Alpha��ֵ
            WICBitmapPaletteTypeCustom       // ��ɫ�����ͣ�ʵ��û��ʹ�ã�����ָ��ΪCustom
        );
        // ����QueryInterface������ö����λͼ����Դ�ӿ�
        pIConverter.As(&pIBMP);
    }
    else
    {
        //ͼƬ���ݸ�ʽ����Ҫת����ֱ�ӻ�ȡ��λͼ����Դ�ӿ�
        pIWICFrame.As(&pIBMP);
    }
    //���ͼƬ��С����λ�����أ�
    pIBMP->GetSize(&nTextureW, &nTextureH);

    //��ȡͼƬ���ص�λ��С��BPP��Bits Per Pixel����Ϣ�����Լ���ͼƬ�����ݵ���ʵ��С����λ���ֽڣ�
    ComPtr<IWICComponentInfo> pIWICmntinfo;
    pIWICFactory->CreateComponentInfo(tgFormat, pIWICmntinfo.GetAddressOf());

    WICComponentType type;
    pIWICmntinfo->GetComponentType(&type);

    if (type != WICPixelFormat)
    {
        return 0;
    }

    ComPtr<IWICPixelFormatInfo> pIWICPixelinfo;
    pIWICmntinfo.As(&pIWICPixelinfo);

    // ���������ڿ��Եõ�BPP�ˣ���Ҳ���ҿ��ıȽ���Ѫ�ĵط���Ϊ��BPP��Ȼ������ô�໷��
    pIWICPixelinfo->GetBitsPerPixel(&nBPP);

    // ����ͼƬʵ�ʵ��д�С����λ���ֽڣ�������ʹ����һ����ȡ����������A+B-1��/B ��
    nPicRowPitch = (uint64_t(nTextureW) * uint64_t(nBPP) + 7u) / 8u;

    //----------------------------------------------------------------------------------

#endif // Render_Base_Rectangle_With_Texture

    

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
        pIPipelineState.Get(),
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
    //����rtvHandle�������Ϊ������������Ϊ����forѭ������RTV
    for (UINT i = 0; i < nFrameBackBufCount; i++)
    {
        pISwapChain3->GetBuffer(i, IID_PPV_ARGS(&pIARenderTargets[i]));
        pID3DDevice->CreateRenderTargetView(pIARenderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, nRTVDescriptorSize);//������offset
    }

#ifdef Render_Base_Triangle

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
#endif // Render_Base_Triangle


#ifdef Render_Base_Rectangle_With_Texture

    //������������
    D3D12_FEATURE_DATA_ROOT_SIGNATURE stFeatureData = {};
    // ����Ƿ�֧��V1.1�汾�ĸ�ǩ��
    stFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(pID3DDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &stFeatureData, sizeof(stFeatureData))))
    {
        stFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    /*//��̬�������汾------------------------------------------------------------------------------
    // ��GPU��ִ��SetGraphicsRootDescriptorTable�󣬲��޸������б��е�SRV����˿���ʹ��Ĭ����Ϊ:
    // D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE
    CD3DX12_DESCRIPTOR_RANGE1 stDSPRanges[1] = {};
    stDSPRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE);

    CD3DX12_ROOT_PARAMETER1 stRootParameters[1] = {};
    stRootParameters[0].InitAsDescriptorTable(1, &stDSPRanges[0], D3D12_SHADER_VISIBILITY_PIXEL);

    //����һ����̬�Ĳ���������
    D3D12_STATIC_SAMPLER_DESC stSamplerDesc = {};
    stSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    stSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    stSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    stSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    stSamplerDesc.MipLODBias = 0;
    stSamplerDesc.MaxAnisotropy = 0;
    stSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    stSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    stSamplerDesc.MinLOD = 0.0f;
    stSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    stSamplerDesc.ShaderRegister = 0;
    stSamplerDesc.RegisterSpace = 0;
    stSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC stRootSignatureDesc = {};
    stRootSignatureDesc.Init_1_1(
        _countof(stRootParameters),
        stRootParameters,
        1,
        &stSamplerDesc,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    //��̬�������汾------------------------------------------------------------------------------
    */
    
    //��̬�������汾------------------------------------------------------------------------------
    //������������
    D3D12_DESCRIPTOR_HEAP_DESC stSamplerHeapDesc = {};
    stSamplerHeapDesc.NumDescriptors = nSampleMaxCnt;
    stSamplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    stSamplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    pID3DDevice->CreateDescriptorHeap(
        &stSamplerHeapDesc,
        IID_PPV_ARGS(&pISamplerDescriptorHeap));

    nSamplerDescriptorSize = pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    CD3DX12_CPU_DESCRIPTOR_HANDLE hSamplerHeap(pISamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    D3D12_SAMPLER_DESC stSamplerDesc = {};
    stSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    stSamplerDesc.MinLOD = 0;
    stSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    stSamplerDesc.MipLODBias = 0.0f;
    stSamplerDesc.MaxAnisotropy = 1;
    stSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    // Sampler 1
    stSamplerDesc.BorderColor[0] = 0.1f;
    stSamplerDesc.BorderColor[1] = 0.4f;
    stSamplerDesc.BorderColor[2] = 0.2f;
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


    CD3DX12_DESCRIPTOR_RANGE1 stDSPRanges[2] = {};
    stDSPRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE);
    stDSPRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

    CD3DX12_ROOT_PARAMETER1 stRootParameters[2] = {};
    stRootParameters[0].InitAsDescriptorTable(1, &stDSPRanges[0], D3D12_SHADER_VISIBILITY_PIXEL);
    stRootParameters[1].InitAsDescriptorTable(1, &stDSPRanges[1], D3D12_SHADER_VISIBILITY_PIXEL);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC stRootSignatureDesc;
    
    stRootSignatureDesc.Init_1_1(
        _countof(stRootParameters),
        stRootParameters,
        0,
        nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    //��̬�������汾------------------------------------------------------------------------------

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

    //����Shader
#if defined(_DEBUG)
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    TCHAR pszShaderFileName[] = _T("D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Shaders\\texture.hlsl");
    D3DCompileFromFile(pszShaderFileName, nullptr, nullptr
        , "VSMain", "vs_5_0", compileFlags, 0, &pIBlobVertexShader, nullptr);
    D3DCompileFromFile(pszShaderFileName, nullptr, nullptr
        , "PSMain", "ps_5_0", compileFlags, 0, &pIBlobPixelShader, nullptr);

    //������������������������벼�ֵĽṹ��
    D3D12_INPUT_ELEMENT_DESC stInputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    //������Ⱦ����״̬����
    D3D12_GRAPHICS_PIPELINE_STATE_DESC stPSODesc = {};
    stPSODesc.InputLayout = { stInputElementDescs, _countof(stInputElementDescs) };
    stPSODesc.pRootSignature = pIRootSignature.Get();
    stPSODesc.VS = CD3DX12_SHADER_BYTECODE(pIBlobVertexShader.Get());
    stPSODesc.PS = CD3DX12_SHADER_BYTECODE(pIBlobPixelShader.Get());
    stPSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    stPSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    stPSODesc.DepthStencilState.DepthEnable = FALSE;
    stPSODesc.DepthStencilState.StencilEnable = FALSE;
    stPSODesc.SampleMask = UINT_MAX;
    stPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    stPSODesc.NumRenderTargets = 1;
    stPSODesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    stPSODesc.SampleDesc.Count = 1;

    pID3DDevice->CreateGraphicsPipelineState(&stPSODesc
        , IID_PPV_ARGS(&pIPipelineState));

    //�������ṹ��
    stTextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    stTextureDesc.MipLevels = 1;
    stTextureDesc.Format = stTextureFormat; //DXGI_FORMAT_R8G8B8A8_UNORM;
    stTextureDesc.Width = nTextureW;
    stTextureDesc.Height = nTextureH;
    stTextureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    stTextureDesc.DepthOrArraySize = 1;
    stTextureDesc.SampleDesc.Count = 1;
    stTextureDesc.SampleDesc.Quality = 0;

    //����Ĭ�϶��ϵ���Դ��������Texture2D��GPU��Ĭ�϶���Դ�ķ����ٶ�������
    //��Ϊ������Դһ���ǲ��ױ����Դ����������ͨ��ʹ���ϴ��Ѹ��Ƶ�Ĭ�϶���
    //�ڴ�ͳ��D3D11����ǰ��D3D�ӿ��У���Щ���̶�����װ�ˣ�����ֻ��ָ������ʱ������ΪĬ�϶� 
    /*//committed�汾------------------------------------------------------------------------------
    
    D3D12_HEAP_PROPERTIES stHeapProp2 = { D3D12_HEAP_TYPE_DEFAULT };
    //����ʹ��CD3DX12_RESOURCE_DESC::Tex2D���򻯽ṹ��ĳ�ʼ��
    pID3DDevice->CreateCommittedResource(
        &stHeapProp2,
        D3D12_HEAP_FLAG_NONE,
        &stTextureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&pITexture));

    //committed�汾------------------------------------------------------------------------------
    */

    //placed�汾------------------------------------------------------------------------------
    
    //���������Ĭ�϶�
    D3D12_HEAP_DESC stTextureHeapDesc = {};
    //Ϊ��ָ������ͼƬ����2����С�Ŀռ䣬����û����ϸȥ�����ˣ�ֻ��ָ����һ���㹻��Ŀռ䣬�����������
    //ʵ��Ӧ����Ҳ��Ҫ�ۺϿ��Ƿ���ѵĴ�С���Ա�������ö�
    stTextureHeapDesc.SizeInBytes = GRS_UPPER(2 * nPicRowPitch * nTextureH, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
    //ָ���ѵĶ��뷽ʽ������ʹ����Ĭ�ϵ�64K�߽���룬��Ϊ������ʱ����ҪMSAA֧��
    stTextureHeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    stTextureHeapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;		//Ĭ�϶�����
    stTextureHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    stTextureHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    //�ܾ���ȾĿ�������ܾ������������ʵ�ʾ�ֻ�������ڷ���ͨ����
    stTextureHeapDesc.Flags = D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_BUFFERS;
    pID3DDevice->CreateHeap(
        &stTextureHeapDesc,
        IID_PPV_ARGS(&pITextureHeap));

    pID3DDevice->CreatePlacedResource(
        pITextureHeap.Get(),
        0,
        &stTextureDesc,       //����ʹ��CD3DX12_RESOURCE_DESC::Tex2D���򻯽ṹ��ĳ�ʼ��
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&pITexture));

    //placed�汾------------------------------------------------------------------------------

    //��ȡ�ϴ�����Դ����Ĵ�С������ߴ�ͨ������ʵ��ͼƬ�ĳߴ�
    n64UploadBufferSize = GetRequiredIntermediateSize(pITexture.Get(), 0, 1);

    // ���������ϴ��������Դ,ע����������Buffer
    // �ϴ��Ѷ���GPU������˵�����Ǻܲ�ģ�
    // ���Զ��ڼ������������������������
    // ͨ�������ϴ���GPU���ʸ���Ч��Ĭ�϶���
    /*//committed�汾------------------------------------------------------------------------------
    D3D12_HEAP_PROPERTIES stHeapProp3 = { D3D12_HEAP_TYPE_UPLOAD };

    D3D12_RESOURCE_DESC stTextureUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(n64UploadBufferSize);


    pID3DDevice->CreateCommittedResource(
        &stHeapProp3,
        D3D12_HEAP_FLAG_NONE,
        &stTextureUploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pITextureUpload));

    //committed�汾------------------------------------------------------------------------------
    */

    //placed�汾------------------------------------------------------------------------------
    
    //�����ϴ���
    D3D12_HEAP_DESC stUploadHeapDesc = {  };
    //�ߴ���Ȼ��ʵ���������ݴ�С��2����64K�߽�����С
    stUploadHeapDesc.SizeInBytes = GRS_UPPER(2 * n64UploadBufferSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
    //ע���ϴ��ѿ϶���Buffer���ͣ����Բ�ָ�����뷽ʽ����Ĭ����64k�߽����
    stUploadHeapDesc.Alignment = 0;
    stUploadHeapDesc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;		//�ϴ�������
    stUploadHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    stUploadHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    //�ϴ��Ѿ��ǻ��壬���԰ڷ���������
    stUploadHeapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
    pID3DDevice->CreateHeap(
        &stUploadHeapDesc,
        IID_PPV_ARGS(&pIUploadHeap));

    D3D12_RESOURCE_DESC stTextureUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(n64UploadBufferSize);
    pID3DDevice->CreatePlacedResource(
        pIUploadHeap.Get(),
        0,
        &stTextureUploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pITextureUpload));

    //placed�汾------------------------------------------------------------------------------

    //������Դ�����С������ʵ��ͼƬ���ݴ洢���ڴ��С
    void* pbPicData = ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, n64UploadBufferSize);
    if (nullptr == pbPicData)
    {
        return 0;
    }

    //��ͼƬ�ж�ȡ������
    pIBMP->CopyPixels(nullptr
        , nPicRowPitch
        , static_cast<UINT>(nPicRowPitch * nTextureH)   //ע���������ͼƬ������ʵ�Ĵ�С�����ֵͨ��С�ڻ���Ĵ�С
        , reinterpret_cast<BYTE*>(pbPicData));


    //��ȡ���ϴ��ѿ����������ݵ�һЩ����ת���ߴ���Ϣ
    //���ڸ��ӵ�DDS�������Ƿǳ���Ҫ�Ĺ���
    UINT64 n64RequiredSize = 0u;
    UINT   nNumSubresources = 1u;  //����ֻ��һ��ͼƬ��������Դ����Ϊ1
    UINT64 n64TextureRowSizes = 0u;
    UINT   nTextureRowNum = 0u;

    stDestDesc = pITexture->GetDesc();

    pID3DDevice->GetCopyableFootprints(&stDestDesc
        , 0
        , nNumSubresources
        , 0
        , &stTxtLayouts
        , &nTextureRowNum
        , &n64TextureRowSizes
        , &n64RequiredSize);

    //��Ϊ�ϴ���ʵ�ʾ���CPU�������ݵ�GPU���н�
    //�������ǿ���ʹ����Ϥ��Map����������ӳ�䵽CPU�ڴ��ַ��
    //Ȼ�����ǰ��н����ݸ��Ƶ��ϴ�����
    //��Ҫע�����֮���԰��п�������ΪGPU��Դ���д�С
    //��ʵ��ͼƬ���д�С���в����,���ߵ��ڴ�߽����Ҫ���ǲ�һ����
    BYTE* pData = nullptr;
    pITextureUpload->Map(0, NULL, reinterpret_cast<void**>(&pData));

    BYTE* pDestSlice = reinterpret_cast<BYTE*>(pData) + stTxtLayouts.Offset;
    const BYTE* pSrcSlice = reinterpret_cast<const BYTE*>(pbPicData);
    for (UINT y = 0; y < nTextureRowNum; ++y)
    {
        memcpy(pDestSlice + static_cast<SIZE_T>(stTxtLayouts.Footprint.RowPitch) * y
            , pSrcSlice + static_cast<SIZE_T>(nPicRowPitch) * y
            , nPicRowPitch);
    }
    //ȡ��ӳ�� �����ױ��������ÿ֡�ı任��������ݣ�������������Unmap�ˣ�
    //������פ�ڴ�,������������ܣ���Ϊÿ��Map��Unmap�Ǻܺ�ʱ�Ĳ���
    //��Ϊ�������붼��64λϵͳ��Ӧ���ˣ���ַ�ռ����㹻�ģ�������ռ�ò���Ӱ��ʲô
    pITextureUpload->Unmap(0, NULL);

    //�ͷ�ͼƬ���ݣ���һ���ɾ��ĳ���Ա
    ::HeapFree(::GetProcessHeap(), 0, pbPicData);


    //��������з������ϴ��Ѹ����������ݵ�Ĭ�϶ѵ�����
    CD3DX12_TEXTURE_COPY_LOCATION Dst(pITexture.Get(), 0);
    CD3DX12_TEXTURE_COPY_LOCATION Src(pITextureUpload.Get(), stTxtLayouts);
    pICommandList->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);

    //����һ����Դ���ϣ�ͬ����ȷ�ϸ��Ʋ������
    //ֱ��ʹ�ýṹ��Ȼ����õ���ʽ
    D3D12_RESOURCE_BARRIER stResBar1 = {};
    stResBar1.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    stResBar1.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    stResBar1.Transition.pResource = pITexture.Get();
    stResBar1.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    stResBar1.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    stResBar1.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    stResBar1.Transition.pResource = pITexture.Get();

    pICommandList->ResourceBarrier(1, &stResBar1);
    //---------------------------------------------------------------------------------------------
    // ִ�������б��ȴ�������Դ�ϴ���ɣ���һ���Ǳ����
    pICommandList->Close();
    ID3D12CommandList* ppCommandLists[] = { pICommandList.Get() };
    pICommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    //�ȴ�������Դ��ʽ�������
    const UINT64 fence = n64FenceValue;
    pICommandQueue->Signal(pIFence.Get(), fence);
    n64FenceValue++;
    //---------------------------------------------------------------------------------------------
    // ��������û������ִ�е�Χ����ǵ����û�о������¼�ȥ�ȴ���ע��ʹ�õ���������ж����ָ��
    if (pIFence->GetCompletedValue() < fence)
    {
        pIFence->SetEventOnCompletion(fence, hFenceEvent);
        WaitForSingleObject(hFenceEvent, INFINITE);
    }

    //�����������Resetһ�£��ղ��Ѿ�ִ�й���һ���������������
    pICommandAllocator->Reset();
    //Reset�����б�������ָ�������������PSO����
    pICommandList->Reset(pICommandAllocator.Get(), pIPipelineState.Get());

    //---------------------------------------------------------------------------------------------

    //����SRV�� (Shader Resource View Heap)
    D3D12_DESCRIPTOR_HEAP_DESC stSRVHeapDesc = {};
    stSRVHeapDesc.NumDescriptors = 1;
    stSRVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    stSRVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    pID3DDevice->CreateDescriptorHeap(&stSRVHeapDesc, IID_PPV_ARGS(&pISRVHeap));
    //����SRV������
    D3D12_SHADER_RESOURCE_VIEW_DESC stSRVDesc = {};
    stSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    stSRVDesc.Format = stTextureDesc.Format;
    stSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    stSRVDesc.Texture2D.MipLevels = 1;
    pID3DDevice->CreateShaderResourceView(pITexture.Get(), &stSRVDesc, pISRVHeap->GetCPUDescriptorHandleForHeapStart());


    //�������㻺��
    // ���������ε�3D���ݽṹ
    GRS_VERTEX stTriangleVertices[] =
    {
        { { -0.25f * fAspectRatio, -0.25f * fAspectRatio, 0.0f}, { 0.0f, 3.0f } },	// Bottom left.
        { { -0.25f * fAspectRatio, 0.25f * fAspectRatio, 0.0f}, { 0.0f, 0.0f } },	// Top left.
        { { 0.25f * fAspectRatio, -0.25f * fAspectRatio, 0.0f }, { 3.0f, 3.0f } },	// Bottom right.
        { { 0.25f * fAspectRatio, 0.25f * fAspectRatio, 0.0f}, { 3.0f, 0.0f } },    // Top right.
    };

    const UINT nVertexBufferSize = sizeof(stTriangleVertices);

    D3D12_RESOURCE_DESC stResSesc = {};
    stResSesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    stResSesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    stResSesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    stResSesc.Format = DXGI_FORMAT_UNKNOWN;
    stResSesc.Width = nVertexBufferSize;
    stResSesc.Height = 1;
    stResSesc.DepthOrArraySize = 1;
    stResSesc.MipLevels = 1;
    stResSesc.SampleDesc.Count = 1;
    stResSesc.SampleDesc.Quality = 0;

    /*//committed�汾------------------------------------------------------------------------------

    D3D12_HEAP_PROPERTIES stHeapProp1 = { D3D12_HEAP_TYPE_UPLOAD };//Upload�ĸ�����Ǵ�CPU�ڴ��ϴ����Դ��е���˼
    pID3DDevice->CreateCommittedResource(
        &stHeapProp1,
        D3D12_HEAP_FLAG_NONE,
        &stResSesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pIVertexBuffer));

    //committed�汾------------------------------------------------------------------------------
    */
    
    //ʹ�����ϴ��������ݻ�����ͬ��һ���ϴ���
    //ע��ڶ�������ָ���˶��е�ƫ��λ��
    //���նѱ߽�����Ҫ��ʹ��GRS_UPPER������ƫ��λ�ö��뵽��64k�ı߽���
    //placed�汾------------------------------------------------------------------------------

    D3D12_RESOURCE_DESC stVertexUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(nVertexBufferSize);
    pID3DDevice->CreatePlacedResource(
        pIUploadHeap.Get(),//ʹ�õ��Ǻ��ϴ��������ݻ�����ͬ��һ����
        GRS_UPPER(n64UploadBufferSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT),//��ƫ��
        &stVertexUploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pIVertexBuffer));

    //placed�汾------------------------------------------------------------------------------

    //�������ļ�Ϊ�õ�Ҫ���ƵĶ���buffer���������潫�������ݴ�CPU�ڴ����Դ���д���
    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE stReadRange(0, 0);
    pIVertexBuffer->Map(0, &stReadRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, stTriangleVertices, sizeof(stTriangleVertices));
    pIVertexBuffer->Unmap(0, nullptr);

    //��䶥��buffer��ͼ�Ľṹ�壬����GPU����������Դʵ����Vertex Buffer
    stVertexBufferView.BufferLocation = pIVertexBuffer->GetGPUVirtualAddress();
    stVertexBufferView.StrideInBytes = sizeof(GRS_VERTEX);
    stVertexBufferView.SizeInBytes = nVertexBufferSize;

#endif // Render_Base_Rectangle_With_Texture

    

    



#ifdef Render_Base_Triangle

    //����Shader
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



    //������Ⱦ����״̬����ӿ�

    //������������������벼�ֵĽṹ��
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

#endif // Render_Base_Triangle


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
    if (phWait)
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

#ifdef Render_Base_Rectangle_With_Texture
            /*//��̬�������汾------------------------------------------------------------------------------
            ID3D12DescriptorHeap* ppHeaps[] = { pISRVHeap.Get() };
            pICommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
            pICommandList->SetGraphicsRootDescriptorTable(0, pISRVHeap->GetGPUDescriptorHandleForHeapStart());
            //��̬�������汾------------------------------------------------------------------------------
            */

            //��̬�������汾------------------------------------------------------------------------------
            ID3D12DescriptorHeap* ppHeaps[] = { pISRVHeap.Get(),pISamplerDescriptorHeap.Get() };
            pICommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
            pICommandList->SetGraphicsRootDescriptorTable(0, pISRVHeap->GetGPUDescriptorHandleForHeapStart());

            CD3DX12_GPU_DESCRIPTOR_HANDLE hGPUSampler(
                pISamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart(),
                nCurrentSamplerNO,
                nSamplerDescriptorSize);

            pICommandList->SetGraphicsRootDescriptorTable(1, hGPUSampler);
            //��̬�������汾------------------------------------------------------------------------------
#endif // Render_Base_Rectangle_With_Texture

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
            pICommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            pICommandList->IASetVertexBuffers(0, 1, &stVertexBufferView);
            //Draw Call
#ifdef Render_Base_Triangle
            pICommandList->DrawInstanced(3, 1, 0, 0);
#endif // Render_Base_Triangle

#ifdef Render_Base_Rectangle_With_Texture
            pICommandList->DrawInstanced(_countof(stTriangleVertices), 1, 0, 0);
#endif // Render_Base_Rectangle_With_Texture

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