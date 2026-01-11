#pragma once
#include <d3d12.h>
#include <wrl/client.h>

//创建描述符堆的方法在这里。没有放到device里写是因为交换链可能有多个，比如多窗口这种。所以决定让窗口类去管理
class DX12Device;
class DX12DescriptorHeap
{
public:
	DX12DescriptorHeap();
	~DX12DescriptorHeap();


	//拷贝禁用掉
	DX12DescriptorHeap(const DX12DescriptorHeap&) = delete;
	DX12DescriptorHeap& operator=(const DX12DescriptorHeap&) = delete;


	//初始化
	bool Initialize(
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		UINT numDescriptors,
		D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE
	);

	//获取堆的接口
	ID3D12DescriptorHeap* Get() const { return m_heap.Get(); }

	//获取cpu的句柄
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleStart() const;

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT index) const;

	//gpu的同上
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandleStart() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(UINT index) const;


	//描述符堆的数量
	UINT GetDescriptorCount() const { return m_numDescriptors; }

	//类型
	D3D12_DESCRIPTOR_HEAP_TYPE GetType() const { return m_type; }

	//大小
	UINT GetDescriptorSize() const { return m_descriptorSize; }

	bool IsInitialized() const { return m_heap != nullptr; }




private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;
	D3D12_DESCRIPTOR_HEAP_TYPE m_type{};
	UINT m_numDescriptors = 0;
	UINT m_descriptorSize = 0;


};

