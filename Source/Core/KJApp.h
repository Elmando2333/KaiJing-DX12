#pragma once

#include <Windows.h>
#include <string>
#include "Timer/GameTimer.h"
#include "DX12/DX12Device.h"
#include "DX12/DX12SwapChain.h"
#include "DX12/DX12DescriptorHeap.h"
#include "DX12/DX12DepthStencilBuffer.h"
#include "DX12/DX12Viewport.h"

//仿照d3d那个应用程序框架基类，封装了窗口创建、消息循环、DX12初始化等基础功能
//用户后面继承这个类并重写虚函数来实现自己的应用逻辑
class KJApp
{
public:
	KJApp(HINSTANCE hInstance);
	virtual ~KJApp();

	//禁用拷贝
	KJApp(const KJApp&) = delete;
	KJApp& operator=(const KJApp&) = delete;

	//主循环
	int Run();

	//窗口句柄和应用实例
	HWND GetMainWindow() const { return m_mainWindow; }
	HINSTANCE GetAppInstance() const { return m_appInstance; }

	//获取一下客户区宽高
	UINT GetClientWidth() const { return m_clientWidth; }
	UINT GetClientHeight() const { return m_clientHeight; }

	//获取DX12的相关对象的引用
	DX12Device& GetDevice() { return DX12Device::GetInstance(); }
	DX12SwapChain& GetSwapChain() { return m_swapChain; }
	DX12DescriptorHeap& GetRTVHeap() { return m_rtvHeap; }
	DX12DescriptorHeap& GetDSVHeap() { return m_dsvHeap; }
	DX12DepthStencilBuffer& GetDepthStencilBuffer() { return m_depthStencilBuffer; }
	DX12Viewport& GetViewport() { return m_viewport; }
	GameTimer& GetTimer() { return m_timer; }

	//检查一下应用状态
	bool IsPaused() const { return m_isPaused; }
	bool IsMinimized() const { return m_isMinimized; }

	//设置窗口标题和尺寸
	void SetWindowTitle(const std::string& title) { m_windowTitle = title; }
	void SetClientWidth(UINT width) { m_clientWidth = width; }
	void SetClientHeight(UINT height) { m_clientHeight = height; }

protected:

	bool m_imguiInitialized = false;
	void SetImGuiInitialized(bool initialized) { m_imguiInitialized = initialized; }

	//后面需要重写的虚函数
	virtual bool Initialize();  //初始化，在窗口和DX12创建后调用
	virtual void Update(float deltaTime);  //每帧更新逻辑
	virtual void Draw();  //每帧绘制逻辑
	virtual void OnResize();  //窗口大小改变时调用

	//输入事件回调，用户可以选择重写
	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}
	virtual void OnKeyDown(WPARAM key) {}
	virtual void OnKeyUp(WPARAM key) {}

private:
	//内部初始化方法
	bool InitializeWindow();  //创建窗口
	bool InitializeDirectX();  //初始化DX12相关资源
	void OnResizeInternal();  //内部处理窗口大小改变
	void CalculateFrameStats();  //计算并显示FPS到标题栏

	//窗口消息处理
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);  //静态窗口过程
	LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);  //实例消息处理

	//窗口相关
	HINSTANCE m_appInstance = nullptr;
	HWND m_mainWindow = nullptr;

	//窗口属性
	std::string m_windowTitle = "KJing Engine";
	UINT m_clientWidth = 1280;
	UINT m_clientHeight = 720;
	bool m_isPaused = false;
	bool m_isMinimized = false;
	bool m_isMaximized = false;
	bool m_isResizing = false;

	//DX12相关资源
	DX12SwapChain m_swapChain;
	DX12DescriptorHeap m_rtvHeap;  //渲染目标视图描述符堆
	DX12DescriptorHeap m_dsvHeap;  //深度模板视图描述符堆
	DX12DepthStencilBuffer m_depthStencilBuffer;
	DX12Viewport m_viewport;

	//时间相关
	GameTimer m_timer;
	float m_deltaTime = 0.0f;

	//FPS统计的
	int m_frameCount = 0;
	float m_timeElapsed = 0.0f;
	float m_fps = 0.0f;
	float m_milliSeconds = 0.0f;
};
