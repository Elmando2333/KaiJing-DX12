#pragma once
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include "DX12Fence.h"




/*
此代码是照着DX12龙书第四章给的参考初始化流程写的,后续还会修改
*/

class DX12Device
{
public :

	static DX12Device& GetInstance();//单例！

	//初始化设备
	bool Initialize(bool enableDebug = true);//完成

	//获取一下原生接口
	ID3D12Device* GetDevice() const { return m_device.Get(); }
	IDXGIFactory4* GetFactory() const { return m_factory.Get(); }

	//获取描述符的大小
	UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const;

	//主围栏
	DX12Fence& GetMainFence() { return m_mainFence; }

	//检测msaa质量支持
	UINT Get4XMSAAQualityLevel() const { return m_msaa4xQualityLevel; }


	//命令和命令队列相关的方法：
	ID3D12CommandQueue* GetCommandQueue() const { return m_commandQueue.Get(); }
	ID3D12CommandAllocator* GetCommandAllocator() const { return m_commandAllocator.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return m_commandList.Get(); }

	void ResetCommandList();



	


private:
	DX12Device() = default;
	~DX12Device();
	
	
	DX12Device(const DX12Device&) = delete;
	DX12Device& operator=(const DX12Device&) = delete;

	DX12Fence m_mainFence;
	

	Microsoft::WRL::ComPtr<ID3D12Device> m_device;
	Microsoft::WRL::ComPtr<IDXGIFactory4> m_factory;

	//开一下调试
#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> m_debugController;
#endif

	
	//缓存描述符大小的
	UINT m_rtvDescriptorSize=0;
	UINT m_dsvDescriptorSize=0;
	UINT m_cbvSrvUavDescriptorSize=0;

	

	//检查4xmsaa质量的
	UINT m_msaa4xQualityLevel = 0;

	

	//命令和命令列表相关的成员
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

};

