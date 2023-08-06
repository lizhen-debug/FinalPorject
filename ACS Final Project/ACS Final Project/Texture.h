#pragma once

//----------------------------------------------------------------------------------
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

struct WICConvert
{// WIC ���ظ�ʽת����
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

//���ȷ�����ݵ���ӽ���ʽ���ĸ�
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

//���ȷ�����ն�Ӧ��DXGI��ʽ����һ��
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


struct stImageInfo
{
    string          m_pszTextureFile;
    DXGI_FORMAT     m_emTextureFormat;
    UINT            m_nTextureW;
    UINT            m_nTextureH;
    UINT            m_nPicRowPitch;
    BYTE*           m_pbImageData;
    size_t          m_szBufferSize;
};

TCHAR* ConvertStringToTCHAR(const string& str) {
    int length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    if (length == 0) {
        // ת��ʧ��
        return nullptr;
    }

    TCHAR* tcharString = new TCHAR[length];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, tcharString, length);
    return tcharString;
}

class Texture
{
public:
	Texture();
	Texture(Engine engine);
    void LoadTexture(string TexcuteFilePath, D3D12_SRV_DIMENSION type);
    void LoadTextureArray(stImageInfo texArray[], UINT texNum, D3D12_SRV_DIMENSION type);
	~Texture();
    
    ComPtr<ID3D12Resource>				 pITexture;
    UINT nTextureW = 0u;//������
    UINT nTextureH = 0u;//����߶�
    D3D12_SRV_DIMENSION emTextureType = {};
    UINT nTextureArrayNum = 0u;
private:

    void WICLoadImageFunction(stImageInfo *image_info);

	Engine GlobalEngine;

    ComPtr<IWICImagingFactory>			 pIWICFactory;
    ComPtr<IWICBitmapDecoder>			 pIWICDecoder;
    ComPtr<IWICBitmapFrameDecode>		 pIWICFrame;
    ComPtr<IWICBitmapSource>			 pIBMP;

    
    UINT nBPP = 0u;//����ÿ��������ռ��λ��
    UINT nPicRowPitch = 0;//����ʵ�ʵ��д�С����λ���ֽڣ�
    UINT64 n64UploadBufferSize = 0;//�ϴ��ѻ����С

    DXGI_FORMAT stTextureFormat = DXGI_FORMAT_UNKNOWN;//�����ʽ
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

inline void Texture::LoadTexture(string TexcuteFilePath, D3D12_SRV_DIMENSION type)
{
    /*
    //----------------------------------------------------------------------------------
    //ʹ�ô�COM��ʽ����WIC�೧����Ҳ�ǵ���WIC��һ��Ҫ��������
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pIWICFactory));

    //ʹ��WIC�೧����ӿڼ�������ͼƬ�����õ�һ��WIC����������ӿڣ�ͼƬ��Ϣ��������ӿڴ���Ķ�������
    pIWICFactory->CreateDecoderFromFilename(
        TexcuteFilePath,                 // �ļ���
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

    // ����һ��λͼ��ʽ��ͼƬ���ݶ���ӿ�
    if (!InlineIsEqualGUID(wpf, tgFormat))
    {//����жϺ���Ҫ�����ԭWIC��ʽ����ֱ����ת��ΪDXGI��ʽ��ͼƬʱ
     //��Ҫ���ľ���ת��ͼƬ��ʽΪ�ܹ�ֱ�Ӷ�ӦDXGI��ʽ����ʽ
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

    ComPtr<IWICPixelFormatInfo> pIWICPixelinfo;
    pIWICmntinfo.As(&pIWICPixelinfo);

    //�õ�BPP
    pIWICPixelinfo->GetBitsPerPixel(&nBPP);

    //����ͼƬʵ�ʵ��д�С����λ���ֽڣ�������ʹ����һ����ȡ����������A+B-1��/B ��
    nPicRowPitch = (uint64_t(nTextureW) * uint64_t(nBPP) + 7u) / 8u;
    //----------------------------------------------------------------------------------
    */
    
    emTextureType = type;

    stImageInfo image = {};

    image.m_pszTextureFile = TexcuteFilePath;
    WICLoadImageFunction(&image);

    //�������ṹ��
    stTextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    stTextureDesc.MipLevels = 1;
    stTextureDesc.Format = image.m_emTextureFormat; //DXGI_FORMAT_R8G8B8A8_UNORM;
    stTextureDesc.Width = image.m_nTextureW;
    stTextureDesc.Height = image.m_nTextureH;
    stTextureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    stTextureDesc.DepthOrArraySize = 1;
    stTextureDesc.SampleDesc.Count = 1;
    stTextureDesc.SampleDesc.Quality = 0;

    //��������Ĭ�϶�
    D3D12_HEAP_PROPERTIES stHeapDefault = { D3D12_HEAP_TYPE_DEFAULT };
    GlobalEngine.pID3DDevice->CreateCommittedResource(
        &stHeapDefault,
        D3D12_HEAP_FLAG_NONE,
        &stTextureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&pITexture));

    //��ȡ�ϴ�����Դ����Ĵ�С������ߴ�ͨ������ʵ��ͼƬ�ĳߴ�
    n64UploadBufferSize = GetRequiredIntermediateSize(pITexture.Get(), 0, 1);

    D3D12_HEAP_PROPERTIES stHeapUpload = { D3D12_HEAP_TYPE_UPLOAD };

    D3D12_RESOURCE_DESC stTextureUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(n64UploadBufferSize);

    //���������ϴ���
    GlobalEngine.pID3DDevice->CreateCommittedResource(
        &stHeapUpload,
        D3D12_HEAP_FLAG_NONE,
        &stTextureUploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&pITextureUpload));

    /*
    //������Դ�����С������ʵ��ͼƬ���ݴ洢���ڴ��С
    void* pbPicData = ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, n64UploadBufferSize);
  
    //��ͼƬ�ж�ȡ������
    pIBMP->CopyPixels(nullptr
        , nPicRowPitch
        , static_cast<UINT>(nPicRowPitch * nTextureH)   //ע���������ͼƬ������ʵ�Ĵ�С�����ֵͨ��С�ڻ���Ĵ�С
        , reinterpret_cast<BYTE*>(pbPicData));
    */
    //---------------------------------------------------------------------------------------------
    //��ȡ���ϴ��ѿ����������ݵ�һЩ����ת���ߴ���Ϣ
    //���ڸ��ӵ�DDS�������Ƿǳ���Ҫ�Ĺ���
    UINT64 n64RequiredSize = 0u;
    UINT   nNumSubresources = 1u;  //ֻ��һ��ͼƬ��������Դ����Ϊ1
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

    //�ϴ���ʵ�ʾ���CPU�������ݵ�GPU���н�
    //���Կ���ʹ�õ�Map����������ӳ�䵽CPU�ڴ��ַ��
    //Ȼ���н����ݸ��Ƶ��ϴ�����
    //��Ҫע�����֮���԰��п�������ΪGPU��Դ���д�С
    //��ʵ��ͼƬ���д�С���в����,���ߵ��ڴ�߽����Ҫ���ǲ�һ����
    BYTE* pData = nullptr;
    pITextureUpload->Map(0, NULL, reinterpret_cast<void**>(&pData));

    BYTE* pDestSlice = reinterpret_cast<BYTE*>(pData) + stTxtLayouts.Offset;

    //const BYTE* pSrcSlice = reinterpret_cast<const BYTE*>(pbPicData);
    const BYTE* pSrcSlice = image.m_pbImageData;

    for (UINT y = 0; y < nTextureRowNum; ++y)
    {
        memcpy(pDestSlice + static_cast<SIZE_T>(stTxtLayouts.Footprint.RowPitch) * y
            , pSrcSlice + static_cast<SIZE_T>(image.m_nPicRowPitch) * y
            , image.m_nPicRowPitch);
    }
    //ȡ��ӳ�� �����ױ��������ÿ֡�ı任��������ݣ�������������Unmap�ˣ�
    //������פ�ڴ�,������������ܣ���Ϊÿ��Map��Unmap�Ǻܺ�ʱ�Ĳ���
    //��Ϊ�������붼��64λϵͳ��Ӧ���ˣ���ַ�ռ����㹻�ģ�������ռ�ò���Ӱ��ʲô
    pITextureUpload->Unmap(0, NULL);

    //�ͷ�ͼƬ���ݣ���һ���ɾ��ĳ���Ա
    //::HeapFree(::GetProcessHeap(), 0, pbPicData);
    ::HeapFree(::GetProcessHeap(), 0, image.m_pbImageData);
    //---------------------------------------------------------------------------------------------
    //��������з������ϴ��Ѹ����������ݵ�Ĭ�϶ѵ�����
    CD3DX12_TEXTURE_COPY_LOCATION Dst(pITexture.Get(), 0);
    CD3DX12_TEXTURE_COPY_LOCATION Src(pITextureUpload.Get(), stTxtLayouts);
    GlobalEngine.pICommandList->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);

    //����һ����Դ���ϣ�ͬ����ȷ�ϸ��Ʋ������
    //ֱ��ʹ�ýṹ��Ȼ����õ���ʽ
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
    // ִ�������б��ȴ�������Դ�ϴ���ɣ���һ���Ǳ����
    GlobalEngine.pICommandList->Close();
    ID3D12CommandList* ppCommandLists[] = { GlobalEngine.pICommandList.Get() };
    GlobalEngine.pICommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    //
    ////�ȴ�������Դ��ʽ�������
    //const UINT64 fence = GlobalEngine.n64FenceValue;
    //GlobalEngine.pICommandQueue->Signal(GlobalEngine.pIFence.Get(), fence);
    //GlobalEngine.n64FenceValue++;
    //
    ////---------------------------------------------------------------------------------------------
    //// ��������û������ִ�е�Χ����ǵ����û�о������¼�ȥ�ȴ���ע��ʹ�õ���������ж����ָ��
    //if (GlobalEngine.pIFence->GetCompletedValue() < fence)
    //{
    //    GlobalEngine.pIFence->SetEventOnCompletion(fence, GlobalEngine.hFenceEvent);
    //    WaitForSingleObject(GlobalEngine.hFenceEvent, INFINITE);
    //}

    //�����������Resetһ�£��ղ��Ѿ�ִ�й���һ���������������
    GlobalEngine.pICommandAllocator->Reset();
    //Reset�����б�������ָ�������������PSO����
    GlobalEngine.pICommandList->Reset(GlobalEngine.pICommandAllocator.Get(), GlobalEngine.pIPipelineState1.Get());

    //---------------------------------------------------------------------------------------------

    
}

inline void Texture::LoadTextureArray(stImageInfo texArray[], UINT texNum, D3D12_SRV_DIMENSION type)
{
    emTextureType = type;
    nTextureArrayNum= texNum;

    for (int i = 0; i < texNum; i++)
    {
        WICLoadImageFunction(&texArray[i]);
    }

    //�������ṹ��
    stTextureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    stTextureDesc.MipLevels = 1;
    stTextureDesc.Format = texArray[0].m_emTextureFormat; //DXGI_FORMAT_R8G8B8A8_UNORM;
    stTextureDesc.Width = texArray[0].m_nTextureW;
    stTextureDesc.Height = texArray[0].m_nTextureH;
    stTextureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    stTextureDesc.DepthOrArraySize = texNum;
    stTextureDesc.SampleDesc.Count = 1;
    stTextureDesc.SampleDesc.Quality = 0;

    //��������Ĭ�϶�
    D3D12_HEAP_PROPERTIES stHeapDefault = { D3D12_HEAP_TYPE_DEFAULT };
    GlobalEngine.pID3DDevice->CreateCommittedResource(
        &stHeapDefault,
        D3D12_HEAP_FLAG_NONE,
        &stTextureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&pITexture));
    
    if (pITexture)
    {
        //---------------------------------------------------------------------------------------------
        UINT64 n64RequiredSize = 0u;
        UINT   nNumSubresources = texNum;
        UINT64* n64TextureRowSizes = new UINT64[texNum];
        UINT* nTextureRowNum = new UINT[texNum];
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT* stTxtArrayLayouts = new D3D12_PLACED_SUBRESOURCE_FOOTPRINT[texNum];

        stDestDesc = pITexture->GetDesc();

        GlobalEngine.pID3DDevice->GetCopyableFootprints(&stDestDesc
            , 0
            , nNumSubresources
            , 0
            , stTxtArrayLayouts
            , nTextureRowNum
            , n64TextureRowSizes
            , &n64RequiredSize);
        //---------------------------------------------------------------------------------------------

        D3D12_HEAP_PROPERTIES stHeapUpload = { D3D12_HEAP_TYPE_UPLOAD };

        D3D12_RESOURCE_DESC stTextureUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(n64RequiredSize);

        //���������ϴ���
        GlobalEngine.pID3DDevice->CreateCommittedResource(
            &stHeapUpload,
            D3D12_HEAP_FLAG_NONE,
            &stTextureUploadDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&pITextureUpload));

        BYTE* pData = nullptr;
        pITextureUpload->Map(0, NULL, reinterpret_cast<void**>(&pData));

        for (int i = 0; i < texNum; i++)
        {
            D3D12_MEMCPY_DEST stCopyDestData = { pData + stTxtArrayLayouts[i].Offset
                        , stTxtArrayLayouts[i].Footprint.RowPitch
                        , stTxtArrayLayouts[i].Footprint.RowPitch * nTextureRowNum[i]
            };

            for (UINT z = 0; z < stTxtArrayLayouts[i].Footprint.Depth; ++z)
            {// Mipmap
                BYTE* pDestSlice = reinterpret_cast<BYTE*>(stCopyDestData.pData) + stCopyDestData.SlicePitch * z;
                const BYTE* pSrcSlice = texArray[i].m_pbImageData;

                for (UINT y = 0; y < nTextureRowNum[i]; ++y)
                {// Rows
                    memcpy(pDestSlice + stCopyDestData.RowPitch * y,
                        pSrcSlice + texArray[i].m_nPicRowPitch * y,
                        (SIZE_T)n64TextureRowSizes[i]);
                }
            }
        }
        //ȡ��ӳ�� �����ױ��������ÿ֡�ı任��������ݣ�������������Unmap�ˣ�
        //������פ�ڴ�,������������ܣ���Ϊÿ��Map��Unmap�Ǻܺ�ʱ�Ĳ���
        //��Ϊ�������붼��64λϵͳ��Ӧ���ˣ���ַ�ռ����㹻�ģ�������ռ�ò���Ӱ��ʲô
        pITextureUpload->Unmap(0, NULL);

        //�ͷ�ͼƬ���ݣ���һ���ɾ��ĳ���Ա
        for (int i = 0; i < texNum; i++)
        {
            ::HeapFree(::GetProcessHeap(), 0, texArray[i].m_pbImageData);
        }

        for (UINT i = 0; i < nNumSubresources; ++i)
        {// ע��ÿ������Դ����һ��
            D3D12_TEXTURE_COPY_LOCATION stDstCopyLocation = {};
            stDstCopyLocation.pResource = pITexture.Get();
            stDstCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            stDstCopyLocation.SubresourceIndex = i;

            D3D12_TEXTURE_COPY_LOCATION stSrcCopyLocation = {};
            stSrcCopyLocation.pResource = pITextureUpload.Get();
            stSrcCopyLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            stSrcCopyLocation.PlacedFootprint = stTxtArrayLayouts[i];

            GlobalEngine.pICommandList->CopyTextureRegion(&stDstCopyLocation, 0, 0, 0, &stSrcCopyLocation, nullptr);
        }

        //����һ����Դ���ϣ�ͬ����ȷ�ϸ��Ʋ������
        //ֱ��ʹ�ýṹ��Ȼ����õ���ʽ
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
        // ִ�������б��ȴ�������Դ�ϴ���ɣ���һ���Ǳ����
        GlobalEngine.pICommandList->Close();
        ID3D12CommandList* ppCommandLists[] = { GlobalEngine.pICommandList.Get() };
        GlobalEngine.pICommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
        //
        ////�ȴ�������Դ��ʽ�������
        //const UINT64 fence = GlobalEngine.n64FenceValue;
        //GlobalEngine.pICommandQueue->Signal(GlobalEngine.pIFence.Get(), fence);
        //GlobalEngine.n64FenceValue++;
        //
        ////---------------------------------------------------------------------------------------------
        //// ��������û������ִ�е�Χ����ǵ����û�о������¼�ȥ�ȴ���ע��ʹ�õ���������ж����ָ��
        //if (GlobalEngine.pIFence->GetCompletedValue() < fence)
        //{
        //    GlobalEngine.pIFence->SetEventOnCompletion(fence, GlobalEngine.hFenceEvent);
        //    WaitForSingleObject(GlobalEngine.hFenceEvent, INFINITE);
        //}

        //�����������Resetһ�£��ղ��Ѿ�ִ�й���һ���������������
        GlobalEngine.pICommandAllocator->Reset();
        //Reset�����б�������ָ�������������PSO����
        GlobalEngine.pICommandList->Reset(GlobalEngine.pICommandAllocator.Get(), GlobalEngine.pIPipelineState1.Get());

        //---------------------------------------------------------------------------------------------

        delete[] n64TextureRowSizes;
        delete[] nTextureRowNum;
        delete[] stTxtArrayLayouts;

    }
    
}

Texture::~Texture()
{
}

inline void Texture::WICLoadImageFunction(stImageInfo* image_info)
{
    if (image_info->m_pszTextureFile!="")
    {
        //----------------------------------------------------------------------------------
        //ʹ�ô�COM��ʽ����WIC�೧����Ҳ�ǵ���WIC��һ��Ҫ��������
        HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pIWICFactory));

        //ʹ��WIC�೧����ӿڼ�������ͼƬ�����õ�һ��WIC����������ӿڣ�ͼƬ��Ϣ��������ӿڴ���Ķ�������
        pIWICFactory->CreateDecoderFromFilename(
            ConvertStringToTCHAR(image_info->m_pszTextureFile),     // �ļ���
            NULL,                                                   // ��ָ����������ʹ��Ĭ��
            GENERIC_READ,                                           // ����Ȩ��
            WICDecodeMetadataCacheOnDemand,                         // ����Ҫ�ͻ������� 
            &pIWICDecoder                                           // ����������
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

        // ����һ��λͼ��ʽ��ͼƬ���ݶ���ӿ�
        if (!InlineIsEqualGUID(wpf, tgFormat))
        {//����жϺ���Ҫ�����ԭWIC��ʽ����ֱ����ת��ΪDXGI��ʽ��ͼƬʱ
         //��Ҫ���ľ���ת��ͼƬ��ʽΪ�ܹ�ֱ�Ӷ�ӦDXGI��ʽ����ʽ
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

        ComPtr<IWICPixelFormatInfo> pIWICPixelinfo;
        pIWICmntinfo.As(&pIWICPixelinfo);

        //�õ�BPP
        pIWICPixelinfo->GetBitsPerPixel(&nBPP);

        //����ͼƬʵ�ʵ��д�С����λ���ֽڣ�������ʹ����һ����ȡ����������A+B-1��/B ��
        nPicRowPitch = (uint64_t(nTextureW) * uint64_t(nBPP) + 7u) / 8u;
        //----------------------------------------------------------------------------------

        //������Դ�����С������ʵ��ͼƬ���ݴ洢���ڴ��С
        void* pbPicData = ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, (UINT64)(nPicRowPitch * nTextureH));

        //��ͼƬ�ж�ȡ������
        pIBMP->CopyPixels(nullptr
            , nPicRowPitch
            , static_cast<UINT>(nPicRowPitch * nTextureH)   //ע���������ͼƬ������ʵ�Ĵ�С�����ֵͨ��С�ڻ���Ĵ�С
            , reinterpret_cast<BYTE*>(pbPicData));

        image_info->m_pbImageData = reinterpret_cast<BYTE*>(pbPicData);
        image_info->m_nPicRowPitch = nPicRowPitch;
        image_info->m_nTextureW = nTextureW;
        image_info->m_nTextureH = nTextureH;
        image_info->m_emTextureFormat = stTextureFormat;
        image_info->m_szBufferSize = nPicRowPitch * nTextureH;
    }
}
