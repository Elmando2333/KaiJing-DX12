#include "DX12/DX12DepthStencilBuffer.h"
#include "DX12/DX12DescriptorHeap.h"
#include "DX12/DX12Device.h"



DX12DepthStencilBuffer::DX12DepthStencilBuffer() = default;

DX12DepthStencilBuffer::~DX12DepthStencilBuffer()
{

}


bool DX12DepthStencilBuffer::Create(
	UINT width,
	UINT height,
	DXGI_FORMAT format,
	UINT msaaCount,
	UINT msaaQuality
)
{
	if (width == 0 || height == 0)
	{
		return false;
	}

	auto& device = DX12Device::GetInstance();
	ID3D12Device* d3dDevice = device.GetDevice();
	if (!d3dDevice)
	{
		return false;
	}

	m_width = width;
	m_height = height;
	m_format = format;
	m_msaaCount = msaaCount;
	m_msaaQuality = msaaQuality;


	// 创建深度/模板缓冲区资源描述
	D3D12_RESOURCE_DESC depthStencilDesc = {};
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = format;
	depthStencilDesc.SampleDesc.Count = msaaCount;
	depthStencilDesc.SampleDesc.Quality = msaaQuality;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//清除值
	D3D12_CLEAR_VALUE optClear;
	optClear.Format = format;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	//还有堆属性
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CreationNodeMask = 1;
	heapProps.VisibleNodeMask = 1;


	// 在这创建资源
	HRESULT hr = d3dDevice->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(&m_depthStencilBuffer)
	);

	return SUCCEEDED(hr);
}


bool DX12DepthStencilBuffer::Resize(UINT width, UINT height)
{
	if (width == 0 || height == 0)
	{
		return false;
	}

	if (width == m_width && height == m_height)
	{
		return true;
	}
	
	//释放旧资源（先这么写）注意
	m_depthStencilBuffer.Reset();

	return Create(width, height, m_format, m_msaaCount, m_msaaQuality);
}



bool DX12DepthStencilBuffer::CreateDSV(DX12DescriptorHeap& dsvHeap, UINT index)
{
	if (!m_depthStencilBuffer || !dsvHeap.IsInitialized())
	{
		return false;
	}

	
	if (dsvHeap.GetType() != D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
	{
		return false;
	}

	
	auto& device = DX12Device::GetInstance();
	ID3D12Device* d3dDevice = device.GetDevice();
	if (!d3dDevice)
	{
		return false;
	}

	
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap.GetCPUHandle(index);

	
	d3dDevice->CreateDepthStencilView(
		m_depthStencilBuffer.Get(),
		nullptr,
		dsvHandle
	);

	return true;
}


D3D12_CPU_DESCRIPTOR_HANDLE DX12DepthStencilBuffer::GetDSVHandle(
	DX12DescriptorHeap& dsvHeap,
	UINT index) const
{
	if (!dsvHeap.IsInitialized())
	{
		return { 0 };
	}

	return dsvHeap.GetCPUHandle(index);
}