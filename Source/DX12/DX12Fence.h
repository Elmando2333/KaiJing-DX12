#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

//这是围栏封装类，CPU和GPU同步的

class DX12Fence
{
public:
	DX12Fence();
	~DX12Fence();

	
	DX12Fence(const DX12Fence&) = delete;
	DX12Fence& operator=(const DX12Fence&) = delete;

	bool Initialize(ID3D12Device* device, UINT64 initialValue = 0);

	void WaitForValue(UINT64 fenceValue);

	void Signal(ID3D12CommandQueue* queue, UINT64 fenceValue);


	/**
	 * @brief 等待GPU完成所有已提交的工作。TODO：看下vs的多行注释怎么用来着
	 */
	void WaitForIdle();

	void Flush(ID3D12CommandQueue* queue);


	UINT64 GetCompletedValue() const;
	UINT64 GetCurrentValue() const { return m_currentValue; }
	bool IsInitialized() const { return m_fence != nullptr; }


	UINT64 Increment();

	ID3D12Fence* Get() const { return m_fence.Get(); }//获取原生接口


private:
	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
	UINT64 m_currentValue = 0;//当前围栏值（CPU）
	HANDLE m_fenceEvent = nullptr;

	bool CreateEventHandle();

};

