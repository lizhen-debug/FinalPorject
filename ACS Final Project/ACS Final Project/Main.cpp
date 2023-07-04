#include "stdafx.h"
#include "Engine.h"
#include "Scene.h"


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
        USHORT n16KeyCode = (wParam & 0xFF);
        if (VK_SPACE == n16KeyCode)
        {
            //按空格键切换不同的采样器看效果，以明白每种采样器具体的含义
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

    int iWidth = 2560;//窗口宽度
    int iHeight = 1440;//窗口高度

    MSG msg;//窗口的消息

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
    wcex.lpszClassName = TEXT("DirectX12App");;//字符串，用来标识一个窗口类，进程内必须唯一，不可以重名
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);//窗口类的小图标句柄，这里和 hIcon 指定相同的图标

    //注册窗口类，函数接收上述填充的窗口类的指针
    if (!RegisterClassEx(&wcex))
    {
        return 0;
    }
    //创建窗口
    HWND hwnd;

    if (!(hwnd = CreateWindowEx(
        0, //窗口扩展样式（例如让窗口透明），一般直接传空
        TEXT("DirectX12App"), // 填充窗口类 的时候所起的名字，用来建立窗体和窗口类的关联
        TEXT("Hello DirectX12"), //窗口的标题名
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

    Engine GlobalEngine(hwnd);
    Scene scene(GlobalEngine);
    scene.ConstructScene();
   
    //创建定时器对象，以便于创建高效的消息循环
    HANDLE phWait = CreateWaitableTimer(NULL, FALSE, NULL);
    LARGE_INTEGER liDueTime = {};
    liDueTime.QuadPart = -1i64;//1秒后开始计时

    if (phWait)
        SetWaitableTimer(phWait, &liDueTime, 1, NULL, NULL, 0);//可在此设置时间周期

    //记录帧开始时间，和当前时间，以循环结束为界
    ULONGLONG n64tmFrameStart = ::GetTickCount64();
    ULONGLONG n64tmCurrent = n64tmFrameStart;

    //计算旋转角度需要的变量
    double dModelRotationYAngle = 0.0f;

    //开始消息循环，并在其中不断渲染
    DWORD dwRet = 0;
    BOOL bExit = FALSE;
    while (!bExit)
    {
        dwRet = ::MsgWaitForMultipleObjects(1, &phWait, FALSE, 0, QS_ALLINPUT);
        switch (dwRet - WAIT_OBJECT_0)
        {
        case 0:
        case WAIT_TIMEOUT:
        {//计时器时间到


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

        scene.RenderScene();

    }

    return 0;
}