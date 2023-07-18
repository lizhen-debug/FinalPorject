#pragma once
#define NOMINMAX

#include <windows.h>
#include <WindowsX.h>
#include <wincodec.h>
#include <string.h>
#include <tchar.h>
#include <wrl.h>
#include <vector>
#include <fstream>
#include <sstream>


#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include "d3dx12.h"
#include <dxgidebug.h>


using namespace Microsoft;
using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>




//向上取整除法
#define GRS_UPPER_DIV(A,B) ((UINT)(((A)+((B)-1))/(B)))

//更简洁的向上边界对齐算法 内存管理中常用
#define GRS_UPPER(A,B) ((UINT)(((A)+((B)-1))&~(B - 1)))


UINT nCurrentSamplerNO = 2; //当前使用的采样器索引
UINT nSampleMaxCnt = 5;		//创建五个典型的采样器

