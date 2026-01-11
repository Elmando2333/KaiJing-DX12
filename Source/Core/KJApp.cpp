#include "Core/KJApp.h"
#include "Core/KJUtil.h"
#include <sstream>
#include <windowsx.h>
#include <cstdio>
#include <cstring>
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx12.h"

KJApp::KJApp(HINSTANCE hInstance)
	: m_appInstance(hInstance)
{
}

KJApp::~KJApp()
{
	if (m_mainWindow)
	{
		DX12Device::GetInstance().GetMainFence().WaitForIdle();
	}
}

int KJApp::Run()
{
	if (!InitializeWindow())
	{
		return 1;
	}

	if (!InitializeDirectX())
	{
		return 1;
	}

	if (!Initialize())
	{
		return 1;
	}

	m_timer.Reset();

	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (!m_isPaused && !m_isMinimized)
			{
				m_timer.Tick();
				m_deltaTime = m_timer.DeltaTime();

				Update(m_deltaTime);
				Draw();

				CalculateFrameStats();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return static_cast<int>(msg.wParam);
}

bool KJApp::InitializeWindow()
{
	if (!m_appInstance)
	{
		MessageBoxA(nullptr, "Invalid hInstance", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	const char* className = "KaiJingAppClass";
	
	UnregisterClassA(className, m_appInstance);
	SetLastError(0);

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = m_appInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wc.lpszClassName = className;

	ATOM classAtom = RegisterClassExA(&wc);
	if (classAtom == 0)
	{
		DWORD error = GetLastError();
		char errorMsg[256];
		sprintf_s(errorMsg, sizeof(errorMsg), "Failed to register window class. Error: %lu\nhInstance: %p", error, m_appInstance);
		MessageBoxA(nullptr, errorMsg, "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!GetClassInfoExA(m_appInstance, className, &wc))
	{
		char errorMsg[256];
		sprintf_s(errorMsg, sizeof(errorMsg), "Window class not found after registration. Error: %lu", GetLastError());
		MessageBoxA(nullptr, errorMsg, "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int windowX = (screenWidth - static_cast<int>(m_clientWidth)) / 2;
	int windowY = (screenHeight - static_cast<int>(m_clientHeight)) / 2;

	RECT windowRect = { 0, 0, static_cast<LONG>(m_clientWidth), static_cast<LONG>(m_clientHeight) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;

	if (windowWidth <= 0 || windowHeight <= 0)
	{
		char errorMsg[256];
		sprintf_s(errorMsg, sizeof(errorMsg), "Invalid window size: %d x %d", windowWidth, windowHeight);
		MessageBoxA(nullptr, errorMsg, "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	SetLastError(0);
	m_mainWindow = CreateWindowExA(
		0,
		className,
		m_windowTitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		windowX,
		windowY,
		windowWidth,
		windowHeight,
		nullptr,
		nullptr,
		m_appInstance,
		this
	);

	if (!m_mainWindow)
	{
		DWORD error = GetLastError();
		char errorMsg[512];
		sprintf_s(errorMsg, sizeof(errorMsg), 
			"Failed to create window.\nError: %lu\nhInstance: %p\nWidth: %d Height: %d", 
			error, m_appInstance, windowWidth, windowHeight);
		MessageBoxA(nullptr, errorMsg, "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	LONG_PTR style = GetWindowLongPtr(m_mainWindow, GWL_STYLE);
	
	SetWindowLongPtr(m_mainWindow, GWL_STYLE, WS_OVERLAPPEDWINDOW);
	SetWindowPos(m_mainWindow, nullptr, 0, 0, 0, 0, 
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	ShowWindow(m_mainWindow, SW_SHOW);
	UpdateWindow(m_mainWindow);
	
	if (!m_windowTitle.empty())
	{
		SetWindowTextA(m_mainWindow, m_windowTitle.c_str());
	}
	
	style = GetWindowLongPtr(m_mainWindow, GWL_STYLE);
	if ((style & WS_CAPTION) == 0)
	{
		SetWindowLongPtr(m_mainWindow, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(m_mainWindow, nullptr, 0, 0, 0, 0, 
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	return true;
}

bool KJApp::InitializeDirectX()
{
	if (!DX12Device::GetInstance().Initialize())
	{
		MessageBoxA(nullptr, "Failed to initialize DX12 device", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!m_rtvHeap.Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_swapChain.GetBufferCount()))
	{
		return false;
	}

	if (!m_dsvHeap.Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1))
	{
		return false;
	}

	if (!m_swapChain.Initialize(
		DX12Device::GetInstance().GetFactory(),
		DX12Device::GetInstance().GetCommandQueue(),
		m_mainWindow,
		m_clientWidth,
		m_clientHeight
	))
	{
		return false;
	}

	if (!m_swapChain.CreateRTVs(
		DX12Device::GetInstance().GetDevice(),
		m_rtvHeap
	))
	{
		return false;
	}

	if (!m_depthStencilBuffer.Create(
		m_clientWidth,
		m_clientHeight,
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		1,
		0
	))
	{
		return false;
	}

	if (!m_depthStencilBuffer.CreateDSV(m_dsvHeap, 0))
	{
		return false;
	}

	m_viewport.SetDefault(m_clientWidth, m_clientHeight);

	LONG_PTR style = GetWindowLongPtr(m_mainWindow, GWL_STYLE);
	if ((style & WS_CAPTION) == 0)
	{
		SetWindowLongPtr(m_mainWindow, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(m_mainWindow, nullptr, 0, 0, 0, 0, 
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	OnResizeInternal();
	return true;
}

void KJApp::OnResizeInternal()
{
	if (!m_mainWindow || m_clientWidth == 0 || m_clientHeight == 0)
	{
		return;
	}

	auto& device = DX12Device::GetInstance();
	if (!device.GetDevice())
	{
		return;
	}

	device.GetMainFence().WaitForIdle();
	device.ResetCommandList();
	
	m_swapChain.Resize(m_clientWidth, m_clientHeight);
	
	if (!m_swapChain.CreateRTVs(
		device.GetDevice(),
		m_rtvHeap
	))
	{
		return;
	}

	m_depthStencilBuffer.Resize(m_clientWidth, m_clientHeight);
	
	if (!m_depthStencilBuffer.CreateDSV(m_dsvHeap, 0))
	{
		return;
	}

	m_viewport.Resize(static_cast<float>(m_clientWidth), static_cast<float>(m_clientHeight));

	OnResize();//注意注意注意！
}

LRESULT CALLBACK KJApp::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	KJApp* pApp = nullptr;

	if (msg == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pApp = reinterpret_cast<KJApp*>(pCreate->lpCreateParams);
		if (pApp)
		{
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApp));
			return TRUE;
		}
		return FALSE;
	}
	else
	{
		pApp = reinterpret_cast<KJApp*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	if (pApp)
	{
		return pApp->HandleMessage(msg, wParam, lParam);
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT KJApp::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// 只有在 ImGui 已初始化后才让 ImGui 处理消息
	// 这样可以避免在窗口创建阶段（ImGui 未初始化时）调用 ImGui 函数
	if (m_imguiInitialized && ImGui::GetCurrentContext() != nullptr)
	{
		if (ImGui_ImplWin32_WndProcHandler(m_mainWindow, msg, wParam, lParam))
			return true;
	}


	switch (msg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_isPaused = true;
			m_timer.Stop();
		}
		else
		{
			m_isPaused = false;
			m_timer.Start();
		}
		return 0;

	case WM_SIZE:
		m_clientWidth = LOWORD(lParam);
		m_clientHeight = HIWORD(lParam);

		if (wParam == SIZE_MINIMIZED)
		{
			m_isPaused = true;
			m_isMinimized = true;
			m_isMaximized = false;
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			m_isPaused = false;
			m_isMinimized = false;
			m_isMaximized = true;
			OnResizeInternal();
		}
		else if (wParam == SIZE_RESTORED)
		{
			if (m_isMinimized)
			{
				m_isPaused = false;
				m_isMinimized = false;
				OnResizeInternal();
			}
			else if (m_isMaximized)
			{
				m_isPaused = false;
				m_isMaximized = false;
				OnResizeInternal();
			}
			else if (m_isResizing)
			{
			}
			else
			{
				OnResizeInternal();
			}
		}
		return 0;

	case WM_ENTERSIZEMOVE:
		m_isPaused = true;
		m_isResizing = true;
		m_timer.Stop();
		return 0;

	case WM_EXITSIZEMOVE:
		m_isPaused = false;
		m_isResizing = false;
		m_timer.Start();
		OnResizeInternal();
		return 0;

	case WM_GETMINMAXINFO:
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = 200;
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_CLOSE:
		DestroyWindow(m_mainWindow);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_KEYDOWN:
		OnKeyDown(wParam);
		return 0;

	case WM_KEYUP:
		OnKeyUp(wParam);
		return 0;
	}

	return DefWindowProc(m_mainWindow, msg, wParam, lParam);
}

void KJApp::CalculateFrameStats()
{
	m_frameCount++;

	if ((m_timer.TotalTimeSeconds() - m_timeElapsed) >= 1.0f)
	{
		m_fps = static_cast<float>(m_frameCount);
		m_milliSeconds = 1000.0f / m_fps;

		char fpsBuffer[256];
		sprintf_s(fpsBuffer, sizeof(fpsBuffer), "%s    FPS: %d    Frame Time: %.2fms", 
			m_windowTitle.c_str(), 
			static_cast<int>(m_fps), 
			m_milliSeconds);
		SetWindowTextA(m_mainWindow, fpsBuffer);

		m_frameCount = 0;
		m_timeElapsed += 1.0f;
	}
}

bool KJApp::Initialize()
{
	return true;
}

void KJApp::Update(float deltaTime)
{
}

void KJApp::Draw()
{
}

void KJApp::OnResize()
{
}
