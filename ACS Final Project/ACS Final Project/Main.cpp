#include "stdafx.h"

//#define Render_Base_Triangle
#define Render_Base_Rectangle_With_Texture


//新定义的宏用于上取整除法
#define GRS_UPPER_DIV(A,B) ((UINT)(((A)+((B)-1))/(B)))

//更简洁的向上边界对齐算法 内存管理中常用 请记住
#define GRS_UPPER(A,B) ((UINT)(((A)+((B)-1))&~(B - 1)))


using namespace Microsoft;
using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;

#ifdef Render_Base_Rectangle_With_Texture

//DX12示例库中的两个用于格式匹配的工具函数
struct WICTranslate
{
    GUID wic;
    DXGI_FORMAT format;
};

static WICTranslate g_WICFormats[] =
{//WIC格式与DXGI像素格式的对应表，该表中的格式为被支持的格式
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

// WIC 像素格式转换表.
struct WICConvert
{
    GUID source;
    GUID target;
};

static WICConvert g_WICConvert[] =
{
    // 目标格式一定是最接近的被支持的格式
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
{//查表确定兼容的最接近格式是哪个
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
{//查表确定最终对应的DXGI格式是哪一个
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

//窗体备用
HWND g_hwnd;
HINSTANCE g_hInstance;

#ifdef Render_Base_Triangle

//定义顶点结构体
struct GRS_VERTEX
{
    XMFLOAT3 position;//等价于float[3]
    XMFLOAT4 color;//等价于float[4]
};

#endif // Render_Base_Triangle

UINT nCurrentSamplerNO = 0; //当前使用的采样器索引
UINT nSampleMaxCnt = 5;		//创建五个典型的采样器

//过程函数
LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY://销毁窗体
        PostQuitMessage(0);
        break;

    case WM_KEYUP:
    {
        if (VK_SPACE == (wParam & 0xFF))
        {//按空格键切换不同的采样器看效果，以明白每种采样器具体的含义
            //UINT nCurrentSamplerNO = 0; //当前使用的采样器索引
            //UINT nSampleMaxCnt = 5;		//创建五个典型的采样器
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
    //使用com智能指针的初始化，不调用的话有可能出现程序退出时指针释放产生内存访问冲突的异常
    HRESULT hr = CoInitialize(nullptr);

    //每帧缓冲区的数量，双缓冲为2，三缓冲为3，后面的u是后缀，将数字解释为无符号整型
    const UINT nFrameBackBufCount = 3u;
    //每帧缓冲区索引，上面可见每帧缓冲区数量为3，则这里的索引即为0，1，2
    UINT nFrameIndex = 0;


    int iWidth = 1024;//窗口宽度
    int iHeight = 768;//窗口高度

    UINT nFrame = 0;

    //0u指的是：将数字0解释为无符号整数类型，u为后缀
    UINT nDXGIFactoryFlags = 0u;//用于创建DX工厂的flag，因为创建工厂需要提供一个flag

    UINT nRTVDescriptorSize = 0u;//堆描述符的大小

    float fAspectRatio = 3.0f;//用于控制绘制三角形大小的比例值

    MSG msg;//窗口的消息

    //一个用于提供给绘制命令队列的顶点buffer的view，描述了顶点buffer的信息
    D3D12_VERTEX_BUFFER_VIEW stVertexBufferView = {};

    //围栏的标记值，0为数字，ui64为后缀，表示将0指定数值为无符号 64 位整数类型
    UINT64 n64FenceValue = 0ui64;

    //用于存放创建的event对象的指针
    HANDLE hFenceEvent = nullptr;

    //用于提供给绘制命令队列的DX视口，此处直接赋值为：左上角位置（0，0），窗口宽和高
    CD3DX12_VIEWPORT stViewPort(0.0f, 0.0f, static_cast<float>(iWidth), static_cast<float>(iHeight));

    //用于提供给绘制命令队列的裁切矩形，用于限制可见区域，此处大小和位置与视口一致
    CD3DX12_RECT  stScissorRect(0, 0, static_cast<LONG>(iWidth), static_cast<LONG>(iHeight));
    //声明DX变量
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

    //声明WIC变量
    ComPtr<IWICImagingFactory>			 pIWICFactory;
    ComPtr<IWICBitmapDecoder>			 pIWICDecoder;
    ComPtr<IWICBitmapFrameDecode>		 pIWICFrame;
    ComPtr<IWICBitmapSource>			 pIBMP;

    UINT nTextureW = 0u;//纹理宽度
    UINT nTextureH = 0u;//纹理高度
    UINT nBPP = 0u;//纹理每个像素所占的位数
    DXGI_FORMAT stTextureFormat = DXGI_FORMAT_UNKNOWN;//纹理格式
    UINT nPicRowPitch = 0;
    UINT64 n64UploadBufferSize = 0;

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT stTxtLayouts = {};
    D3D12_RESOURCE_DESC stTextureDesc = {};
    D3D12_RESOURCE_DESC stDestDesc = {};

    UINT nSamplerDescriptorSize = 0; //采样器大小

    //----------------------------------------------------------------------------------

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

#ifdef Render_Base_Rectangle_With_Texture

    //----------------------------------------------------------------------------------

    //使用纯COM方式创建WIC类厂对象，也是调用WIC第一步要做的事情
    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pIWICFactory));

    //使用WIC类厂对象接口加载纹理图片，并得到一个WIC解码器对象接口，图片信息就在这个接口代表的对象中了
    TCHAR pszTexcuteFileName[] = _T("D:\\OneDrive - University of Exeter\\MSc Advanced Computer Science\\Code Dir\\ACS Final Project\\FinalPorject\\ACS Final Project\\ACS Final Project\\Textures\\test1.jpg");

    pIWICFactory->CreateDecoderFromFilename(
        pszTexcuteFileName,              // 文件名
        NULL,                            // 不指定解码器，使用默认
        GENERIC_READ,                    // 访问权限
        WICDecodeMetadataCacheOnDemand,  // 若需要就缓冲数据 
        &pIWICDecoder                    // 解码器对象
    );

    //获取第一帧图片，解析出的通常是位图
    pIWICDecoder->GetFrame(0, &pIWICFrame);

    WICPixelFormatGUID wpf = {};
    //获取WIC图片格式
    pIWICFrame->GetPixelFormat(&wpf);
    GUID tgFormat = {};

    //通过第一道转换之后获取DXGI的等价格式
    if (GetTargetPixelFormat(&wpf, &tgFormat))
    {
        stTextureFormat = GetDXGIFormatFromPixelFormat(&tgFormat);
    }

    if (DXGI_FORMAT_UNKNOWN == stTextureFormat)
    {// 不支持的图片格式 目前退出了事 
     // 一般 在实际的引擎当中都会提供纹理格式转换工具，
     // 图片都需要提前转换好，所以不会出现不支持的现象
        return 0;
    }

    // 定义一个位图格式的图片数据对象接口

    if (!InlineIsEqualGUID(wpf, tgFormat))
    {// 这个判断很重要，如果原WIC格式不是直接能转换为DXGI格式的图片时
     // 我们需要做的就是转换图片格式为能够直接对应DXGI格式的形式
        //创建图片格式转换器
        ComPtr<IWICFormatConverter> pIConverter;
        pIWICFactory->CreateFormatConverter(&pIConverter);

        //初始化一个图片转换器，实际也就是将图片数据进行了格式转换
        pIConverter->Initialize(
            pIWICFrame.Get(),                // 输入原图片数据
            tgFormat,						 // 指定待转换的目标格式
            WICBitmapDitherTypeNone,         // 指定位图是否有调色板，现代都是真彩位图，不用调色板，所以为None
            NULL,                            // 指定调色板指针
            0.f,                             // 指定Alpha阀值
            WICBitmapPaletteTypeCustom       // 调色板类型，实际没有使用，所以指定为Custom
        );
        // 调用QueryInterface方法获得对象的位图数据源接口
        pIConverter.As(&pIBMP);
    }
    else
    {
        //图片数据格式不需要转换，直接获取其位图数据源接口
        pIWICFrame.As(&pIBMP);
    }
    //获得图片大小（单位：像素）
    pIBMP->GetSize(&nTextureW, &nTextureH);

    //获取图片像素的位大小的BPP（Bits Per Pixel）信息，用以计算图片行数据的真实大小（单位：字节）
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

    // 到这里终于可以得到BPP了，这也是我看的比较吐血的地方，为了BPP居然饶了这么多环节
    pIWICPixelinfo->GetBitsPerPixel(&nBPP);

    // 计算图片实际的行大小（单位：字节），这里使用了一个上取整除法即（A+B-1）/B ，
    nPicRowPitch = (uint64_t(nTextureW) * uint64_t(nBPP) + 7u) / 8u;

    //----------------------------------------------------------------------------------

#endif // Render_Base_Rectangle_With_Texture

    

    //创建factory，以便为后续部分创建适配器，设备等
    CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(&pIDXGIFactory5));

    //关闭ALT+ENTER键切换全屏的功能，因为没有实现OnSize处理，所以先关闭
    pIDXGIFactory5->MakeWindowAssociation(g_hwnd, DXGI_MWA_NO_ALT_ENTER);

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
        pIPipelineState.Get(),
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
        g_hwnd,
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
    //上面rtvHandle可以理解为迭代器，方便为下面for循环创建RTV
    for (UINT i = 0; i < nFrameBackBufCount; i++)
    {
        pISwapChain3->GetBuffer(i, IID_PPV_ARGS(&pIARenderTargets[i]));
        pID3DDevice->CreateRenderTargetView(pIARenderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, nRTVDescriptorSize);//迭代器offset
    }

#ifdef Render_Base_Triangle

    //创建根签名，描述整个的用于渲染的资源的存储布局
    //整体上统一集中管理分散在各个资源创建函数参数中的存储Slot和对应寄存器序号的对象
    //不用在创建某个资源时单独在其参数中指定对应哪个Slot和寄存器及序号了
    //而是统一在D3D12中用一个根签名就将这些描述清楚
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

    //创建根描述符
    D3D12_FEATURE_DATA_ROOT_SIGNATURE stFeatureData = {};
    // 检测是否支持V1.1版本的根签名
    stFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(pID3DDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &stFeatureData, sizeof(stFeatureData))))
    {
        stFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    /*//静态采样器版本------------------------------------------------------------------------------
    // 在GPU上执行SetGraphicsRootDescriptorTable后，不修改命令列表中的SRV，因此可以使用默认行为:
    // D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE
    CD3DX12_DESCRIPTOR_RANGE1 stDSPRanges[1] = {};
    stDSPRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE);

    CD3DX12_ROOT_PARAMETER1 stRootParameters[1] = {};
    stRootParameters[0].InitAsDescriptorTable(1, &stDSPRanges[0], D3D12_SHADER_VISIBILITY_PIXEL);

    //定义一个静态的采样器对象
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
    //静态采样器版本------------------------------------------------------------------------------
    */
    
    //动态采样器版本------------------------------------------------------------------------------
    //创建采样器堆
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
    //动态采样器版本------------------------------------------------------------------------------

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

    //编译Shader
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

    //声明和填充用于描述顶点输入布局的结构体
    D3D12_INPUT_ELEMENT_DESC stInputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    //创建渲染管线状态对象
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

    //填充纹理结构体
    stTextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    stTextureDesc.MipLevels = 1;
    stTextureDesc.Format = stTextureFormat; //DXGI_FORMAT_R8G8B8A8_UNORM;
    stTextureDesc.Width = nTextureW;
    stTextureDesc.Height = nTextureH;
    stTextureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    stTextureDesc.DepthOrArraySize = 1;
    stTextureDesc.SampleDesc.Count = 1;
    stTextureDesc.SampleDesc.Quality = 0;

    //创建默认堆上的资源，类型是Texture2D，GPU对默认堆资源的访问速度是最快的
    //因为纹理资源一般是不易变的资源，所以我们通常使用上传堆复制到默认堆中
    //在传统的D3D11及以前的D3D接口中，这些过程都被封装了，我们只能指定创建时的类型为默认堆 
    /*//committed版本------------------------------------------------------------------------------
    
    D3D12_HEAP_PROPERTIES stHeapProp2 = { D3D12_HEAP_TYPE_DEFAULT };
    //可以使用CD3DX12_RESOURCE_DESC::Tex2D来简化结构体的初始化
    pID3DDevice->CreateCommittedResource(
        &stHeapProp2,
        D3D12_HEAP_FLAG_NONE,
        &stTextureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&pITexture));

    //committed版本------------------------------------------------------------------------------
    */

    //placed版本------------------------------------------------------------------------------
    
    //创建纹理的默认堆
    D3D12_HEAP_DESC stTextureHeapDesc = {};
    //为堆指定纹理图片至少2倍大小的空间，这里没有详细去计算了，只是指定了一个足够大的空间，够放纹理就行
    //实际应用中也是要综合考虑分配堆的大小，以便可以重用堆
    stTextureHeapDesc.SizeInBytes = GRS_UPPER(2 * nPicRowPitch * nTextureH, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
    //指定堆的对齐方式，这里使用了默认的64K边界对齐，因为我们暂时不需要MSAA支持
    stTextureHeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    stTextureHeapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;		//默认堆类型
    stTextureHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    stTextureHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    //拒绝渲染目标纹理、拒绝深度蜡板纹理，实际就只是用来摆放普通纹理
    stTextureHeapDesc.Flags = D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_BUFFERS;
    pID3DDevice->CreateHeap(
        &stTextureHeapDesc,
        IID_PPV_ARGS(&pITextureHeap));

    pID3DDevice->CreatePlacedResource(
        pITextureHeap.Get(),
        0,
        &stTextureDesc,       //可以使用CD3DX12_RESOURCE_DESC::Tex2D来简化结构体的初始化
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&pITexture));

    //placed版本------------------------------------------------------------------------------

    //获取上传堆资源缓冲的大小，这个尺寸通常大于实际图片的尺寸
    n64UploadBufferSize = GetRequiredIntermediateSize(pITexture.Get(), 0, 1);

    // 创建用于上传纹理的资源,注意其类型是Buffer
    // 上传堆对于GPU访问来说性能是很差的，
    // 所以对于几乎不变的数据尤其像纹理都是
    // 通过它来上传至GPU访问更高效的默认堆中
    /*//committed版本------------------------------------------------------------------------------
    D3D12_HEAP_PROPERTIES stHeapProp3 = { D3D12_HEAP_TYPE_UPLOAD };

    D3D12_RESOURCE_DESC stTextureUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(n64UploadBufferSize);


    pID3DDevice->CreateCommittedResource(
        &stHeapProp3,
        D3D12_HEAP_FLAG_NONE,
        &stTextureUploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pITextureUpload));

    //committed版本------------------------------------------------------------------------------
    */

    //placed版本------------------------------------------------------------------------------
    
    //创建上传堆
    D3D12_HEAP_DESC stUploadHeapDesc = {  };
    //尺寸依然是实际纹理数据大小的2倍并64K边界对齐大小
    stUploadHeapDesc.SizeInBytes = GRS_UPPER(2 * n64UploadBufferSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
    //注意上传堆肯定是Buffer类型，可以不指定对齐方式，其默认是64k边界对齐
    stUploadHeapDesc.Alignment = 0;
    stUploadHeapDesc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;		//上传堆类型
    stUploadHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    stUploadHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    //上传堆就是缓冲，可以摆放任意数据
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

    //placed版本------------------------------------------------------------------------------

    //按照资源缓冲大小来分配实际图片数据存储的内存大小
    void* pbPicData = ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, n64UploadBufferSize);
    if (nullptr == pbPicData)
    {
        return 0;
    }

    //从图片中读取出数据
    pIBMP->CopyPixels(nullptr
        , nPicRowPitch
        , static_cast<UINT>(nPicRowPitch * nTextureH)   //注意这里才是图片数据真实的大小，这个值通常小于缓冲的大小
        , reinterpret_cast<BYTE*>(pbPicData));


    //获取向上传堆拷贝纹理数据的一些纹理转换尺寸信息
    //对于复杂的DDS纹理这是非常必要的过程
    UINT64 n64RequiredSize = 0u;
    UINT   nNumSubresources = 1u;  //我们只有一副图片，即子资源个数为1
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

    //因为上传堆实际就是CPU传递数据到GPU的中介
    //所以我们可以使用熟悉的Map方法将它先映射到CPU内存地址中
    //然后我们按行将数据复制到上传堆中
    //需要注意的是之所以按行拷贝是因为GPU资源的行大小
    //与实际图片的行大小是有差异的,二者的内存边界对齐要求是不一样的
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
    //取消映射 对于易变的数据如每帧的变换矩阵等数据，可以撒懒不用Unmap了，
    //让它常驻内存,以提高整体性能，因为每次Map和Unmap是很耗时的操作
    //因为现在起码都是64位系统和应用了，地址空间是足够的，被长期占用不会影响什么
    pITextureUpload->Unmap(0, NULL);

    //释放图片数据，做一个干净的程序员
    ::HeapFree(::GetProcessHeap(), 0, pbPicData);


    //向命令队列发出从上传堆复制纹理数据到默认堆的命令
    CD3DX12_TEXTURE_COPY_LOCATION Dst(pITexture.Get(), 0);
    CD3DX12_TEXTURE_COPY_LOCATION Src(pITextureUpload.Get(), stTxtLayouts);
    pICommandList->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);

    //设置一个资源屏障，同步并确认复制操作完成
    //直接使用结构体然后调用的形式
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
    // 执行命令列表并等待纹理资源上传完成，这一步是必须的
    pICommandList->Close();
    ID3D12CommandList* ppCommandLists[] = { pICommandList.Get() };
    pICommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    //等待纹理资源正式复制完成
    const UINT64 fence = n64FenceValue;
    pICommandQueue->Signal(pIFence.Get(), fence);
    n64FenceValue++;
    //---------------------------------------------------------------------------------------------
    // 看命令有没有真正执行到围栏标记的这里，没有就利用事件去等待，注意使用的是命令队列对象的指针
    if (pIFence->GetCompletedValue() < fence)
    {
        pIFence->SetEventOnCompletion(fence, hFenceEvent);
        WaitForSingleObject(hFenceEvent, INFINITE);
    }

    //命令分配器先Reset一下，刚才已经执行过了一个复制纹理的命令
    pICommandAllocator->Reset();
    //Reset命令列表，并重新指定命令分配器和PSO对象
    pICommandList->Reset(pICommandAllocator.Get(), pIPipelineState.Get());

    //---------------------------------------------------------------------------------------------

    //创建SRV堆 (Shader Resource View Heap)
    D3D12_DESCRIPTOR_HEAP_DESC stSRVHeapDesc = {};
    stSRVHeapDesc.NumDescriptors = 1;
    stSRVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    stSRVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    pID3DDevice->CreateDescriptorHeap(&stSRVHeapDesc, IID_PPV_ARGS(&pISRVHeap));
    //创建SRV描述符
    D3D12_SHADER_RESOURCE_VIEW_DESC stSRVDesc = {};
    stSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    stSRVDesc.Format = stTextureDesc.Format;
    stSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    stSRVDesc.Texture2D.MipLevels = 1;
    pID3DDevice->CreateShaderResourceView(pITexture.Get(), &stSRVDesc, pISRVHeap->GetCPUDescriptorHandleForHeapStart());


    //创建顶点缓冲
    // 定义正方形的3D数据结构
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

    /*//committed版本------------------------------------------------------------------------------

    D3D12_HEAP_PROPERTIES stHeapProp1 = { D3D12_HEAP_TYPE_UPLOAD };//Upload的概念就是从CPU内存上传到显存中的意思
    pID3DDevice->CreateCommittedResource(
        &stHeapProp1,
        D3D12_HEAP_FLAG_NONE,
        &stResSesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pIVertexBuffer));

    //committed版本------------------------------------------------------------------------------
    */
    
    //使用与上传纹理数据缓冲相同的一个上传堆
    //注意第二个参数指出了堆中的偏移位置
    //按照堆边界对齐的要求，使用GRS_UPPER主动将偏移位置对齐到了64k的边界上
    //placed版本------------------------------------------------------------------------------

    D3D12_RESOURCE_DESC stVertexUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(nVertexBufferSize);
    pID3DDevice->CreatePlacedResource(
        pIUploadHeap.Get(),//使用的是和上传纹理数据缓冲相同的一个堆
        GRS_UPPER(n64UploadBufferSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT),//堆偏移
        &stVertexUploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pIVertexBuffer));

    //placed版本------------------------------------------------------------------------------

    //上述核心即为得到要绘制的顶点buffer，用于下面将绘制数据从CPU内存向显存进行传递
    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE stReadRange(0, 0);
    pIVertexBuffer->Map(0, &stReadRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, stTriangleVertices, sizeof(stTriangleVertices));
    pIVertexBuffer->Unmap(0, nullptr);

    //填充顶点buffer视图的结构体，告诉GPU被描述的资源实际是Vertex Buffer
    stVertexBufferView.BufferLocation = pIVertexBuffer->GetGPUVirtualAddress();
    stVertexBufferView.StrideInBytes = sizeof(GRS_VERTEX);
    stVertexBufferView.SizeInBytes = nVertexBufferSize;

#endif // Render_Base_Rectangle_With_Texture

    

    



#ifdef Render_Base_Triangle

    //编译Shader
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



    //创建渲染管线状态对象接口

    //填充用于描述顶点输入布局的结构体
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }

    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };//_countof宏，方便获取静态数组元素个数
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

    //填充资源结构体及创建提交资源对象
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

    D3D12_HEAP_PROPERTIES stHeapProp = { D3D12_HEAP_TYPE_UPLOAD };//Upload的概念就是从CPU内存上传到显存中的意思
    pID3DDevice->CreateCommittedResource(
        &stHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &stResSesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pIVertexBuffer));
    //上述核心即为得到要绘制的顶点buffer，用于下面将绘制数据从CPU内存向显存进行传递
    UINT8* pVertexDataBegin = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    pIVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
    pIVertexBuffer->Unmap(0, nullptr);

    //填充顶点buffer视图的结构体，告诉GPU被描述的资源实际是Vertex Buffer
    stVertexBufferView.BufferLocation = pIVertexBuffer->GetGPUVirtualAddress();
    stVertexBufferView.StrideInBytes = sizeof(GRS_VERTEX);
    stVertexBufferView.SizeInBytes = vertexBufferSize;

#endif // Render_Base_Triangle


    // 填充开始和结束两个资源屏障结构体
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



    //创建定时器对象，以便于创建高效的消息循环
    HANDLE phWait = CreateWaitableTimer(NULL, FALSE, NULL);
    LARGE_INTEGER liDueTime = {};

    liDueTime.QuadPart = -1i64;//1秒后开始计时
    if (phWait)
        SetWaitableTimer(phWait, &liDueTime, 1000, NULL, NULL, 0);//可在此设置时间周期，单位为毫秒
        //此处1000毫秒指的是每过一秒进行一次消息循环
    //开始消息循环，并在其中不断渲染
    DWORD dwRet = 0;
    BOOL bExit = FALSE;
    while (!bExit)
    {
        dwRet = ::MsgWaitForMultipleObjects(1, &phWait, FALSE, INFINITE, QS_ALLINPUT);
        switch (dwRet - WAIT_OBJECT_0)
        {
        case 0:
        case WAIT_TIMEOUT:
        {//计时器时间到
             //开始记录命令
            pICommandList->SetGraphicsRootSignature(pIRootSignature.Get());

#ifdef Render_Base_Rectangle_With_Texture
            /*//静态采样器版本------------------------------------------------------------------------------
            ID3D12DescriptorHeap* ppHeaps[] = { pISRVHeap.Get() };
            pICommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
            pICommandList->SetGraphicsRootDescriptorTable(0, pISRVHeap->GetGPUDescriptorHandleForHeapStart());
            //静态采样器版本------------------------------------------------------------------------------
            */

            //动态采样器版本------------------------------------------------------------------------------
            ID3D12DescriptorHeap* ppHeaps[] = { pISRVHeap.Get(),pISamplerDescriptorHeap.Get() };
            pICommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
            pICommandList->SetGraphicsRootDescriptorTable(0, pISRVHeap->GetGPUDescriptorHandleForHeapStart());

            CD3DX12_GPU_DESCRIPTOR_HANDLE hGPUSampler(
                pISamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart(),
                nCurrentSamplerNO,
                nSamplerDescriptorSize);

            pICommandList->SetGraphicsRootDescriptorTable(1, hGPUSampler);
            //动态采样器版本------------------------------------------------------------------------------
#endif // Render_Base_Rectangle_With_Texture

            pICommandList->RSSetViewports(1, &stViewPort);
            pICommandList->RSSetScissorRects(1, &stScissorRect);


            // 通过资源屏障判定后缓冲已经切换完毕可以开始渲染了
            stBeginResBarrier.Transition.pResource = pIARenderTargets[nFrameIndex].Get();
            pICommandList->ResourceBarrier(1, &stBeginResBarrier);

            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pIRTVHeap->GetCPUDescriptorHandleForHeapStart()
                , nFrameIndex
                , nRTVDescriptorSize);

            //设置渲染目标
            pICommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
            // 继续记录命令，并真正开始新一帧的渲染
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
            const UINT64 fence = n64FenceValue;
            pICommandQueue->Signal(pIFence.Get(), fence);
            n64FenceValue++;

            // 看命令有没有真正执行到围栏标记的这里，没有就利用事件去等待，注意使用的是命令队列对象的指针
            if (pIFence->GetCompletedValue() < fence)
            {
                pIFence->SetEventOnCompletion(fence, hFenceEvent);
                WaitForSingleObject(hFenceEvent, INFINITE);
            }

            //到这里说明一个命令队列完整的执行完了，在这里就代表我们的一帧已经渲染完了，接着准备执行下一帧//渲染
            //获取新的后缓冲序号，因为Present真正完成时后缓冲的序号就更新了
            nFrameIndex = pISwapChain3->GetCurrentBackBufferIndex();
            //命令分配器先Reset一下
            pICommandAllocator->Reset();
            //Reset命令列表，并重新指定命令分配器和PSO对象
            pICommandList->Reset(pICommandAllocator.Get(), pIPipelineState.Get());


            nFrame++;
            wstring str1 = L"第";
            wstring str2 = to_wstring(nFrame);
            wstring str3 = L"帧绘制完成\n";
            wstring result = str1 + str2 + str3;
            LPCWSTR frameInfo = result.c_str();
            OutputDebugString(frameInfo);

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
        default:
            break;
        }

    }

    return 0;
}