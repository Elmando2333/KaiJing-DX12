// DX12Device.cpp

#include "DX12Device.h"
#include <stdexcept>



DX12Device& DX12Device::GetInstance()
{
	//TODO:这里是不是要再判断一下存不存在再开始创建？
	static DX12Device instance;
	return instance;
}


bool DX12Device::Initialize(bool enableDebug)
{
	HRESULT hr;//就是hardwareResult的缩写，简写方便点

#ifdef DEBUG
	if (enableDebug)
	{
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugController));
		if (SUCCEEDED(hr))
		{
			m_debugController->EnableDebugLayer();
		}
	}
#endif // DEBUG

	
	hr = CreateDXGIFactory(IID_PPV_ARGS(&m_factory));
	if (FAILED(hr)) return false;

	hr = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_12_0,
		IID_PPV_ARGS(&m_device)
	);


	if (FAILED(hr))
	{


		//回退WARP
		Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
		hr = m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
		if (FAILED(hr)) return false;

		//用WARP来创建啊
		hr = D3D12CreateDevice(
			warpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)
		);
	}


	if (FAILED(hr))
	{
		return false;
	}



	//查询一下设备的特性
	D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
	m_device->CheckFeatureSupport(
		D3D12_FEATURE_D3D12_OPTIONS,
		&options,
		sizeof(options)
	);

	//检测msaa质量
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;

	if (m_device)
	{
		m_device->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&msQualityLevels,
			sizeof(msQualityLevels)
		);

		m_msaa4xQualityLevel = msQualityLevels.NumQualityLevels;
	}



	//下面是创建围栏的
	if (m_device)
	{
		if (!m_mainFence.Initialize(m_device.Get()))
		{
			return false;
		}
	}



	//缓存描述符大小？
	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV
	);
	m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV
	);
	m_cbvSrvUavDescriptorSize = m_device->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
	);




	//命令队列相关的初始化
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
	if (FAILED(hr)) return false;

	//命令列表分配器
	hr = m_device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&m_commandAllocator)
	);
	if (FAILED(hr)) return false;

	//命令列表
	hr = m_device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_commandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&m_commandList)
	);
	if (FAILED(hr)) return false;

	m_commandList->Close();



	


	////这里初始化描述符堆,rtv这里注释掉了，因为想让rtv的创建跟着交换链走
	//D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	//rtvHeapDesc.NumDescriptors = m_swapChain.GetBufferCount();
	//rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	//rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	//rtvHeapDesc.NodeMask = 0;






	return true;
}


UINT DX12Device::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const
{
	if (!m_device) return 0;

	
	switch (type)
	{
	case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
		return m_rtvDescriptorSize;
	case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
		return m_dsvDescriptorSize;
	case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
		return m_cbvSrvUavDescriptorSize;
	default:
		return m_device->GetDescriptorHandleIncrementSize(type);
	}
}

DX12Device::~DX12Device()
{
	
	// 先等待GPU完成所有工作
	if (m_device)
	{
		m_mainFence.WaitForIdle();
	}
	
	
}



void DX12Device::ResetCommandList()
{
	m_commandAllocator->Reset();
	m_commandList->Reset(m_commandAllocator.Get(), nullptr);
}


