#pragma once

#include <windows.h>
#include <WindowsX.h>
#include <wincodec.h>
#include <string.h>
#include <tchar.h>
#include <wrl.h>
#include <vector>


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


//向上取整除法
#define GRS_UPPER_DIV(A,B) ((UINT)(((A)+((B)-1))/(B)))

//更简洁的向上边界对齐算法 内存管理中常用
#define GRS_UPPER(A,B) ((UINT)(((A)+((B)-1))&~(B - 1)))


UINT nCurrentSamplerNO = 0; //当前使用的采样器索引
UINT nSampleMaxCnt = 5;		//创建五个典型的采样器
//物体旋转的角速度，单位：弧度/秒
double fPalstance = 10.0f * XM_PI / 180.0f;
