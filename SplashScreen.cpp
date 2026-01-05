#include <Windows.h>
#include <tchar.h>
#include <string>
#include <DirectXMath.h>


//句柄
static HWND g_mainWindow = nullptr;

//窗口尺寸？
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

//biaoti
const char* WINDOW_TITLE = "开境引擎";




LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam);

bool RegisterWindowClass(HINSTANCE hInstance);

HWND CreateMainWindow(HINSTANCE hInstance);



void RunMessageLoop();


int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow);




bool RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEX wc = {};

    wc.cbSize = sizeof(WNDCLASSEX);        // 结构体大小
    wc.style = CS_HREDRAW | CS_VREDRAW;    // 窗口样式
    wc.lpfnWndProc = WindowProc;           // 窗口过程函数指针
    wc.hInstance = hInstance;              // 应用程序实例
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); // 光标
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // 背景色
    wc.lpszClassName = "MyEngineSplashClass"; // 窗口类名


    return RegisterClassEx(&wc) != 0;
}


HWND CreateMainWindow(HINSTANCE hInstance)
{
    // 窗口居中的位置
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowX = (screenWidth - WINDOW_WIDTH) / 2;
    int windowY = (screenHeight - WINDOW_HEIGHT) / 2;

    // 创建窗口（交给AI写）
    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST,                              // 扩展样式（暂时用0）
        "MyEngineSplashClass",          // 窗口类名（和注册的一致）
        WINDOW_TITLE,                   // 窗口标题
        WS_POPUP,            // 窗口样式（标准窗口）
        windowX, windowY,               // 窗口位置
        WINDOW_WIDTH, WINDOW_HEIGHT,    // 窗口大小
        NULL,                           // 父窗口
        NULL,                           // 菜单
        hInstance,                      // 应用程序实例
        NULL                            // 创建参数
    );

    return hwnd;
}




//窗口过程啊
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        // 关闭窗口
        PostQuitMessage(0);  
        return 0;

    case WM_PAINT:
    {
        // 绘制窗口内容（暂时留空）
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // 在这里绘制

        EndPaint(hwnd, &ps);
    }
    return 0;

    
    }

    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}




void RunMessageLoop()
{
    MSG msg = {};

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}


int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    if (!RegisterWindowClass(hInstance))
    {
        MessageBox(NULL, "窗口类注册失败!", "错误", MB_ICONERROR);
        return 1;
    }

    // 2. 创建窗口
    g_mainWindow = CreateMainWindow(hInstance);
    if (!g_mainWindow)
    {
        MessageBox(NULL, "窗口创建失败!", "错误", MB_ICONERROR);
        return 1;
    }

    // 3. 显示窗口
    ShowWindow(g_mainWindow, nCmdShow);
    UpdateWindow(g_mainWindow);  // 强制立即绘制

    // 4. 运行消息循环
    RunMessageLoop();





    return 0;
}