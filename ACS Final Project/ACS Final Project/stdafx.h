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


//����ȡ������
#define GRS_UPPER_DIV(A,B) ((UINT)(((A)+((B)-1))/(B)))

//���������ϱ߽�����㷨 �ڴ�����г���
#define GRS_UPPER(A,B) ((UINT)(((A)+((B)-1))&~(B - 1)))


UINT nCurrentSamplerNO = 0; //��ǰʹ�õĲ���������
UINT nSampleMaxCnt = 5;		//����������͵Ĳ�����
//������ת�Ľ��ٶȣ���λ������/��
double fPalstance = 10.0f * XM_PI / 180.0f;
