#include "DX12DescriptorHeap.h"
#include "DX12Device.h"

DX12DescriptorHeap::DX12DescriptorHeap() = default;
DX12DescriptorHeap::~DX12DescriptorHeap()
{

}


bool DX12DescriptorHeap::Initialize(
	D3D12_DESCRIPTOR_HEAP_TYPE type,
	UINT numDescriptors,
	D3D12_DESCRIPTOR_HEAP_FLAGS flags
)
{

	if (numDescriptors == 0)
	{
		return false;
	}

	auto& device = DX12Device::GetInstance();
	ID3D12Device* d3dDevice = device.GetDevice();

	if (!d3dDevice)
	{
		return false;
	}

	m_type = type;
	m_numDescriptors = numDescriptors;
	m_descriptorSize = device.GetDescriptorHandleIncrementSize(type);
	if (m_descriptorSize == 0)
	{
		return false;
	}


	//开始创建描述符堆（抄龙书的）但不用rtv，dsv分开创建重复写代码了
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};//question:发现爱用列表初始化，好处是啥来着：避免未初始化的问题，所有成员初始化为0
	heapDesc.NumDescriptors = m_numDescriptors;
	heapDesc.Type = m_type;
	heapDesc.Flags = flags;
	heapDesc.NodeMask = 0;

	HRESULT hr = d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_heap));
	if (FAILED(hr))
	{
		return false;
	}


	return true;
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12DescriptorHeap::GetCPUHandleStart() const
{
	if (!m_heap)
	{
		return { 0 };
	}

	return m_heap->GetCPUDescriptorHandleForHeapStart();
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12DescriptorHeap::GetCPUHandle(UINT index) const
{
	if (!m_heap || index >= m_numDescriptors)
	{
		return { 0 };
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_heap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += m_descriptorSize * index;
	return handle;
}

//下面是GPU的
D3D12_GPU_DESCRIPTOR_HANDLE DX12DescriptorHeap::GetGPUHandleStart() const
{
	if (!m_heap)
	{
		return { 0 };
	}

	return m_heap->GetGPUDescriptorHandleForHeapStart();
}

D3D12_GPU_DESCRIPTOR_HANDLE DX12DescriptorHeap::GetGPUHandle(UINT index) const
{
	if (!m_heap || index >= m_numDescriptors)
	{
		return { 0 };
	}

	D3D12_GPU_DESCRIPTOR_HANDLE handle = m_heap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += m_descriptorSize * index;
	return handle;
}