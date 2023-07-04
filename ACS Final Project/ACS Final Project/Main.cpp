#include "stdafx.h"
#include "Engine.h"
#include "Scene.h"


//���̺���
LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY://���ٴ���
        PostQuitMessage(0);
        break;

    case WM_KEYUP:
    {
        USHORT n16KeyCode = (wParam & 0xFF);
        if (VK_SPACE == n16KeyCode)
        {
            //���ո���л���ͬ�Ĳ�������Ч����������ÿ�ֲ���������ĺ���
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

    int iWidth = 2560;//���ڿ��
    int iHeight = 1440;//���ڸ߶�

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

    if (!(hwnd = CreateWindowEx(
        0, //������չ��ʽ�������ô���͸������һ��ֱ�Ӵ���
        TEXT("DirectX12App"), // ��䴰���� ��ʱ����������֣�������������ʹ�����Ĺ���
        TEXT("Hello DirectX12"), //���ڵı�����
        WS_OVERLAPPED | WS_SYSMENU,//���ڵ���ʽ
        0, //X��λ��
        0, //Y��λ��
        iWidth, //���ڿ��
        iHeight, //���ڸ߶�
        NULL, //�Ƿ��и�����
        NULL, //�Ƿ����в˵�
        hInstance, //����Ӧ��ʵ��������� WinMain ���� �ĵ�һ������
        NULL//���Ӳ���
    )))
    {
        return 0;
    }
    //��ʾ���´���
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    Engine GlobalEngine(hwnd);
    Scene scene(GlobalEngine);
    scene.ConstructScene();
   
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

        scene.RenderScene();

    }

    return 0;
}