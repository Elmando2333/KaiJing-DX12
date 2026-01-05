#include "DX12SwapChain.h"

DX12SwapChain::DX12SwapChain() = default;

DX12SwapChain::~DX12SwapChain()
{
}

bool DX12SwapChain::Initialize(IDXGIFactory4* factory, ID3D12CommandQueue* commandQueue, HWND hwnd, UINT width, UINT height, DXGI_FORMAT format)
{
	if (!factory || !commandQueue || !hwnd)
	{
		return false;
	}

	m_backBufferFormat = format;
	m_width = width;
	m_height = height;

	//下面是一大串交换链的描述
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = format;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = m_bufferCount;
	sd.OutputWindow = hwnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//然后创建交换链子
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	HRESULT hr = factory->CreateSwapChain(
		commandQueue,
		&sd,
		swapChain.GetAddressOf()
	);
	if (FAILED(hr)) return false;

	hr = swapChain.As(&m_swapChain);
	if (FAILED(hr)) return false;

	return true;
}

bool DX12SwapChain::Resize(UINT width, UINT height)
{
	if (!m_swapChain || width == 0 || height == 0)
	{
		return false;
	}

	m_width = width;
	m_height = height;

	HRESULT hr = m_swapChain->ResizeBuffers(
		m_bufferCount,
		width,
		height,
		m_backBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	);
	return SUCCEEDED(hr);
}

void DX12SwapChain::Present(UINT syncInterval, UINT flags)
{
	if (m_swapChain)
	{
		m_swapChain->Present(syncInterval, flags);
	}
}

UINT DX12SwapChain::GetCurrentBackBufferIndex() const
{
	if (!m_swapChain)
	{
		return 0;
	}

	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain3;
	if (SUCCEEDED(m_swapChain.As(&swapChain3)))
	{
		return swapChain3->GetCurrentBackBufferIndex();
	}

	return 0;
}

ID3D12Resource* DX12SwapChain::GetBackBuffer(UINT index) const
{
	if (!m_swapChain)
	{
		return nullptr;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
	HRESULT hr = m_swapChain->GetBuffer(index, IID_PPV_ARGS(&backBuffer));
	if (FAILED(hr))
	{
		return nullptr;
	}

	return backBuffer.Detach();
}
