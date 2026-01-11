#pragma once

#include <d3d12.h>
#include <wrl/client.h>


class DX12Device;
class DX12DescriptorHeap;


class DX12DepthStencilBuffer
{
public:
	DX12DepthStencilBuffer();
	~DX12DepthStencilBuffer();


	//禁用拷贝
	DX12DepthStencilBuffer(const DX12DepthStencilBuffer&) = delete;
	DX12DepthStencilBuffer& operator=(const DX12DepthStencilBuffer&) = delete;


	//创建深度缓冲区
	bool Create(
		UINT width,
		UINT height,
		DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT,
		UINT msaaCount = 1,
		UINT msaaQuality=0
	);

	bool Resize(UINT width, UINT height);

	//创建dsv视图
	bool CreateDSV(DX12DescriptorHeap& dsvHeap, UINT index = 0);


	//资源接口
	ID3D12Resource* Get() const { return m_depthStencilBuffer.Get(); }

	//获取句柄
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle(DX12DescriptorHeap& dsvHeap, UINT index = 0) const;

	//格式
	DXGI_FORMAT GetFormat() const { return m_format; }

	//宽高
	UINT GetWidth() const { return m_width; }
	UINT GetHeight() const { return m_height; }

	bool IsCreated() const { return m_depthStencilBuffer != nullptr; }


private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencilBuffer;
	DXGI_FORMAT m_format= DXGI_FORMAT_D24_UNORM_S8_UINT;
	UINT m_width = 0;
	UINT m_height = 0;
	UINT m_msaaCount = 1;
	UINT m_msaaQuality = 0;
};

