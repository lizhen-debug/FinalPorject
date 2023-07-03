#pragma once

#include "Engine.h"


extern Engine GlobalEngine;

//全局窗口变量
HWND g_hwnd;
HINSTANCE g_hInstance;

UINT nCurrentSamplerNO = 0; //当前使用的采样器索引
UINT nSampleMaxCnt = 5;		//创建五个典型的采样器
