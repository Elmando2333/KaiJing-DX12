#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>


class DX12SwapChain
{
public:
	DX12SwapChain();
	~DX12SwapChain();

	//把拷贝禁用了
	DX12SwapChain(const DX12SwapChain&) = delete;
	DX12SwapChain& operator=(const DX12SwapChain&) = delete;

	bool Initialize(
		IDXGIFactory4* factory,
		ID3D12CommandQueue* commandQueue,
		HWND hwnd,
		UINT width,
		UINT height,
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM
	);


	bool Resize(UINT width, UINT height);

	//交换前后的缓冲区
	void Present(UINT syncInterval = 1, UINT flags = 0);

	//获取即将交换的缓冲区的索引
	UINT GetCurrentBackBufferIndex() const;

	//获取资源，后台的
	ID3D12Resource* GetBackBuffer(UINT index) const;


	//获取接口
	IDXGISwapChain* Get() const { return m_swapChain.Get(); }
	UINT GetBufferCount() const { return m_bufferCount; }

	//获取格式
	DXGI_FORMAT GetFormat() const { return m_backBufferFormat; }

	//获取宽高
	UINT GetWidth() const { return m_width; }
	UINT GetHeight() const { return m_height; }

	bool IsInitialized() const { return m_swapChain != nullptr; }



private:
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;

	DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	UINT m_width = 0;
	UINT m_height = 0;
	UINT m_bufferCount = 2;
};


