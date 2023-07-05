#pragma once

//----------------------------------------------------------------------------------
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

struct WICConvert
{// WIC 像素格式转换表
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

//查表确定兼容的最接近格式是哪个
bool GetTargetPixelFormat(const GUID* pSourceFormat, GUID* pTargetFormat)
{
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

//查表确定最终对应的DXGI格式是哪一个
DXGI_FORMAT GetDXGIFormatFromPixelFormat(const GUID* pPixelFormat)
{
    for (size_t i = 0; i < _countof(g_WICFormats); ++i)
    {
        if (InlineIsEqualGUID(g_WICFormats[i].wic, *pPixelFormat))
        {
            return g_WICFormats[i].format;
        }
    }
    return DXGI_FORMAT_UNKNOWN;
}


class Texture
{
public:
	Texture();
	Texture(Engine engine);
    void LoadTexture(TCHAR TexcuteFilePath[]);
	~Texture();
    
    ComPtr<ID3D12Resource>				 pITexture;

private:
	Engine GlobalEngine;

    ComPtr<IWICImagingFactory>			 pIWICFactory;
    ComPtr<IWICBitmapDecoder>			 pIWICDecoder;
    ComPtr<IWICBitmapFrameDecode>		 pIWICFrame;
    ComPtr<IWICBitmapSource>			 pIBMP;

    UINT nTextureW = 0u;//纹理宽度
    UINT nTextureH = 0u;//纹理高度
    UINT nBPP = 0u;//纹理每个像素所占的位数
    UINT nPicRowPitch = 0;//纹理实际的行大小（单位：字节）
    UINT64 n64UploadBufferSize = 0;//上传堆缓冲大小

    DXGI_FORMAT stTextureFormat = DXGI_FORMAT_UNKNOWN;//纹理格式
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT stTxtLayouts = {};
    D3D12_RESOURCE_DESC stDestDesc = {};
    D3D12_RESOURCE_DESC stTextureDesc = {};

    
    
    ComPtr<ID3D12Resource>				 pITextureUpload;
};

Texture::Texture()
{
}

Texture::Texture(Engine engine)
{
	GlobalEngine = engine;
}

inline void Texture::LoadTexture(TCHAR TexcuteFilePath[])
{
    //----------------------------------------------------------------------------------
    //使用纯COM方式创建WIC类厂对象，也是调用WIC第一步要做的事情
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pIWICFactory));

    //使用WIC类厂对象接口加载纹理图片，并得到一个WIC解码器对象接口，图片信息就在这个接口代表的对象中了
    pIWICFactory->CreateDecoderFromFilename(
        TexcuteFilePath,                 // 文件名
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

    // 定义一个位图格式的图片数据对象接口
    if (!InlineIsEqualGUID(wpf, tgFormat))
    {//这个判断很重要，如果原WIC格式不是直接能转换为DXGI格式的图片时
     //需要做的就是转换图片格式为能够直接对应DXGI格式的形式
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

    ComPtr<IWICPixelFormatInfo> pIWICPixelinfo;
    pIWICmntinfo.As(&pIWICPixelinfo);

    //得到BPP
    pIWICPixelinfo->GetBitsPerPixel(&nBPP);

    //计算图片实际的行大小（单位：字节），这里使用了一个上取整除法即（A+B-1）/B ，
    nPicRowPitch = (uint64_t(nTextureW) * uint64_t(nBPP) + 7u) / 8u;
    //----------------------------------------------------------------------------------

    

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

    //创建纹理默认堆
    D3D12_HEAP_PROPERTIES stHeapDefault = { D3D12_HEAP_TYPE_DEFAULT };
    GlobalEngine.pID3DDevice->CreateCommittedResource(
        &stHeapDefault,
        D3D12_HEAP_FLAG_NONE,
        &stTextureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&pITexture));

    //获取上传堆资源缓冲的大小，这个尺寸通常大于实际图片的尺寸
    n64UploadBufferSize = GetRequiredIntermediateSize(pITexture.Get(), 0, 1);

    D3D12_HEAP_PROPERTIES stHeapUpload = { D3D12_HEAP_TYPE_UPLOAD };

    D3D12_RESOURCE_DESC stTextureUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(n64UploadBufferSize);

    //创建纹理上传堆
    GlobalEngine.pID3DDevice->CreateCommittedResource(
        &stHeapUpload,
        D3D12_HEAP_FLAG_NONE,
        &stTextureUploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pITextureUpload));

    //按照资源缓冲大小来分配实际图片数据存储的内存大小
    void* pbPicData = ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, n64UploadBufferSize);
  
    //从图片中读取出数据
    pIBMP->CopyPixels(nullptr
        , nPicRowPitch
        , static_cast<UINT>(nPicRowPitch * nTextureH)   //注意这里才是图片数据真实的大小，这个值通常小于缓冲的大小
        , reinterpret_cast<BYTE*>(pbPicData));

    //---------------------------------------------------------------------------------------------
    //获取向上传堆拷贝纹理数据的一些纹理转换尺寸信息
    //对于复杂的DDS纹理这是非常必要的过程
    UINT64 n64RequiredSize = 0u;
    UINT   nNumSubresources = 1u;  //只有一副图片，即子资源个数为1
    UINT64 n64TextureRowSizes = 0u;
    UINT   nTextureRowNum = 0u;

    stDestDesc = pITexture->GetDesc();

    GlobalEngine.pID3DDevice->GetCopyableFootprints(&stDestDesc
        , 0
        , nNumSubresources
        , 0
        , &stTxtLayouts
        , &nTextureRowNum
        , &n64TextureRowSizes
        , &n64RequiredSize);

    //上传堆实际就是CPU传递数据到GPU的中介
    //所以可以使用的Map方法将它先映射到CPU内存地址中
    //然后按行将数据复制到上传堆中
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

    //---------------------------------------------------------------------------------------------
    //向命令队列发出从上传堆复制纹理数据到默认堆的命令
    CD3DX12_TEXTURE_COPY_LOCATION Dst(pITexture.Get(), 0);
    CD3DX12_TEXTURE_COPY_LOCATION Src(pITextureUpload.Get(), stTxtLayouts);
    GlobalEngine.pICommandList->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);

    //设置一个资源屏障，同步并确认复制操作完成
    //直接使用结构体然后调用的形式
    D3D12_RESOURCE_BARRIER stResBar = {};
    stResBar.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    stResBar.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    stResBar.Transition.pResource = pITexture.Get();
    stResBar.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    stResBar.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    stResBar.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    stResBar.Transition.pResource = pITexture.Get();

    GlobalEngine.pICommandList->ResourceBarrier(1, &stResBar);

    //---------------------------------------------------------------------------------------------
    // 执行命令列表并等待纹理资源上传完成，这一步是必须的
    GlobalEngine.pICommandList->Close();
    ID3D12CommandList* ppCommandLists[] = { GlobalEngine.pICommandList.Get() };
    GlobalEngine.pICommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    //等待纹理资源正式复制完成
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

    //命令分配器先Reset一下，刚才已经执行过了一个复制纹理的命令
    GlobalEngine.pICommandAllocator->Reset();
    //Reset命令列表，并重新指定命令分配器和PSO对象
    GlobalEngine.pICommandList->Reset(GlobalEngine.pICommandAllocator.Get(), GlobalEngine.pIPipelineState1.Get());

    //---------------------------------------------------------------------------------------------

    
}

Texture::~Texture()
{
}