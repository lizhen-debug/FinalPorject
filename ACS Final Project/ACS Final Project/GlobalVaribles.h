#pragma once

#include "Engine.h"


extern Engine GlobalEngine;

//ȫ�ִ��ڱ���
HWND g_hwnd;
HINSTANCE g_hInstance;

UINT nCurrentSamplerNO = 0; //��ǰʹ�õĲ���������
UINT nSampleMaxCnt = 5;		//����������͵Ĳ�����
