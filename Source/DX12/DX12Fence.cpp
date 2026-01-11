#include "DX12/DX12Fence.h"
#include <stdexcept>


DX12Fence::DX12Fence() = default;

DX12Fence::~DX12Fence()
{
	if (m_fenceEvent!=nullptr)//记得检查释放
	{
		CloseHandle(m_fenceEvent);
		m_fenceEvent = nullptr;
	}

	
}



bool DX12Fence::Initialize(ID3D12Device* device, UINT64 initialValue)
{
	if (!device)
	{
		return false;
	}

	HRESULT hr = device->CreateFence(
		initialValue,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&m_fence)
	);

	if (FAILED(hr))
	{
		return false;
	}

	m_currentValue = initialValue;

	//创建事件对象等待
	if (!CreateEventHandle())
	{
		return false;
	}

	return true;
}

bool DX12Fence::CreateEventHandle()
{
	m_fenceEvent = CreateEventEx(
		nullptr,
		nullptr,
		CREATE_EVENT_MANUAL_RESET,
		EVENT_ALL_ACCESS
		);

	return m_fenceEvent != nullptr;
}


void DX12Fence::WaitForValue(UINT64 fenceValue)
{
	if (!m_fence || !m_fenceEvent)
	{
		return;
	}

	
	if (m_fence->GetCompletedValue() >= fenceValue)
	{
		return;
	}

	// 设置事件等待完成
	HRESULT hr = m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent);
	if (FAILED(hr))
	{
		// 轮询
		while (m_fence->GetCompletedValue() < fenceValue)
		{
			Sleep(1); // 避免CPU占用过高
		}
		return;
	}

	// 等待事件触发
	WaitForSingleObject(m_fenceEvent, INFINITE);
	ResetEvent(m_fenceEvent);
}

void DX12Fence::Signal(ID3D12CommandQueue* queue, UINT64 fenceValue)
{
	if (!queue || !m_fence)
	{
		return;
	}

	HRESULT hr = queue->Signal(m_fence.Get(), fenceValue);
	if (SUCCEEDED(hr))
	{
		m_currentValue = fenceValue;
	}

}


void DX12Fence::WaitForIdle()
{
	WaitForValue(m_currentValue);
}


void DX12Fence::Flush(ID3D12CommandQueue* queue)
{
	if (!queue || !m_fence)
	{
		return;
	}

	UINT64 fenceValue = Increment();
	Signal(queue, fenceValue);

	WaitForValue(fenceValue);
}


UINT64 DX12Fence::GetCompletedValue() const
{
	if (!m_fence)
	{
		return 0;
	}
	return m_fence->GetCompletedValue();
}


UINT64 DX12Fence::Increment()
{
	return ++m_currentValue;
}