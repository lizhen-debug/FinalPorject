#include "stdafx.h"
#include "Engine.h"
#include "Scene.h"

Scene *scene_object;
static POINT prevMousePos = {};  // ���ڴ洢��һ�ε����λ��

void OnKeyboardInput()
{
    const float dt = 0.01f;

    if (GetAsyncKeyState('W') & 0x8000)
        scene_object->camera.Walk(10.0f * dt);

    if (GetAsyncKeyState('S') & 0x8000)
        scene_object->camera.Walk(-10.0f * dt);

    if (GetAsyncKeyState('A') & 0x8000)
        scene_object->camera.Strafe(-10.0f * dt);

    if (GetAsyncKeyState('D') & 0x8000)
        scene_object->camera.Strafe(10.0f * dt);
}

//���̺���
LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    const float dt = 0.1f;
    switch (uMsg)
    {
    case WM_DESTROY://���ٴ���
        PostQuitMessage(0);
    break;

    case WM_INPUT:
    {
        UINT dataSize = 0;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));
        LPBYTE rawData = new BYTE[dataSize];
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, rawData, &dataSize, sizeof(RAWINPUTHEADER));
       
        RAWINPUT* rawInput = (RAWINPUT*)rawData;
        if (rawInput->header.dwType == RIM_TYPEMOUSE)
        {
            // ���������������
            RAWMOUSE& rawMouse = rawInput->data.mouse;

            if (rawMouse.usFlags == MOUSE_MOVE_RELATIVE)
            {
                int dx = rawMouse.lLastX;
                int dy = rawMouse.lLastY;

                // ��������ƶ���ƫ����
                scene_object->camera.Pitch(dy * 0.001);
                scene_object->camera.RotateY(dx * 0.001);

                // ������һ�ε����λ��
                prevMousePos.x += dx;
                prevMousePos.y += dy;
            }
        }

        delete[] rawData;
    }
    break;
    //case WM_MOUSEMOVE:
    //{
    //    int xPos = GET_X_LPARAM(lParam);  // ��ȡ��굱ǰ�� x ����
    //    int yPos = GET_Y_LPARAM(lParam);  // ��ȡ��굱ǰ�� y ����
    //
    //    // ��������ƶ���ƫ����
    //    int dx = xPos - prevMousePos.x;
    //    int dy = yPos - prevMousePos.y;
    //    scene_object->camera.Pitch(dy*0.001);
    //    scene_object->camera.RotateY(dx*0.001);
    //
    //    // ������һ�ε����λ��
    //    prevMousePos.x = xPos;
    //    prevMousePos.y = yPos;
    //}
    //break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}



int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{

    int iWidth = 1600;//���ڿ��
    int iHeight = 900;//���ڸ߶�

    MSG msg;//���ڵ���Ϣ

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
    wcex.lpszClassName = TEXT("DirectX12App");;//�ַ�����������ʶһ�������࣬�����ڱ���Ψһ������������
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);//�������Сͼ����������� hIcon ָ����ͬ��ͼ��

    //ע�ᴰ���࣬���������������Ĵ������ָ��
    if (!RegisterClassEx(&wcex))
    {
        return 0;
    }
    //��������
    HWND hwnd;

    RECT windowRect = { 0, 0, iWidth, iHeight };
    //AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    if (!(hwnd = CreateWindowEx(
        0, //������չ��ʽ�������ô���͸������һ��ֱ�Ӵ���
        TEXT("DirectX12App"), // ��䴰���� ��ʱ����������֣�������������ʹ�����Ĺ���
        TEXT("Hello DirectX12"), //���ڵı�����
        WS_OVERLAPPED | WS_SYSMENU,//���ڵ���ʽ
        0, //X��λ��
        0, //Y��λ��
        windowWidth, //���ڿ��
        windowHeight, //���ڸ߶�
        NULL, //�Ƿ��и�����
        NULL, //�Ƿ����в˵�
        hInstance, //����Ӧ��ʵ��������� WinMain ���� �ĵ�һ������
        NULL//���Ӳ���
    )))
    {
        return 0;
    }

    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    // ���㴰�ڿͻ�������������
    int centerX = (clientRect.right - clientRect.left) / 2;
    int centerY = (clientRect.bottom - clientRect.top) / 2;

    // �����ָ���ƶ����ͻ���������
    POINT centerPoint = { centerX, centerY };
    ClientToScreen(hwnd, &centerPoint);
    SetCursorPos(centerPoint.x, centerPoint.y);

    prevMousePos.x = centerPoint.x;
    prevMousePos.y = centerPoint.y;
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    RAWINPUTDEVICE rid = {};
    rid.usUsagePage = 0x01; // ͨ�����������
    rid.usUsage = 0x02;     // ���
    rid.dwFlags = 0;
    rid.hwndTarget = hwnd; // hwnd �Ǵ��ڵľ��
    RegisterRawInputDevices(&rid, 1, sizeof(rid));
    

    //��ʾ���´���
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    

    Engine GlobalEngine(hwnd);
    Scene scene(GlobalEngine);
    scene.ConstructScene();
    scene_object = &scene;
   
    //������ʱ�������Ա��ڴ�����Ч����Ϣѭ��
    HANDLE phWait = CreateWaitableTimer(NULL, FALSE, NULL);
    LARGE_INTEGER liDueTime = {};
    liDueTime.QuadPart = -1i64;//1���ʼ��ʱ

    if (phWait)
        SetWaitableTimer(phWait, &liDueTime, 1, NULL, NULL, 0);//���ڴ�����ʱ������

    //��¼֡��ʼʱ�䣬�͵�ǰʱ�䣬��ѭ������Ϊ��
    ULONGLONG n64tmFrameStart = ::GetTickCount64();
    ULONGLONG n64tmCurrent = n64tmFrameStart;

    //������ת�Ƕ���Ҫ�ı���
    double dModelRotationYAngle = 0.0f;

    //��ʼ��Ϣѭ�����������в�����Ⱦ
    DWORD dwRet = 0;
    BOOL bExit = FALSE;
    while (!bExit)
    {
        dwRet = ::MsgWaitForMultipleObjects(1, &phWait, FALSE, 0, QS_ALLINPUT);
        switch (dwRet - WAIT_OBJECT_0)
        {
        case 0:
        case WAIT_TIMEOUT:
        {//��ʱ��ʱ�䵽


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

        OnKeyboardInput();

        scene.RenderList[2].RotationY(0.004);
        scene.RenderScene();

    }

    return 0;
}