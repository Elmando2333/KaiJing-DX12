# KaiJing开境引擎 - DirectX 12 

基于《Introduction to 3D Game Programming with DirectX 12》，非原书代码，自己重新按照书上的流程重新封装

## 已完成

### 核心类

- **DX12Device** (`DX12Device.h/cpp`)
  - DirectX 12 设备管理类（单例模式）
  - 管理设备、命令队列、命令列表
  - MSAA 质量级别检测
  - 描述符大小缓存

- **DX12Fence** (`DX12Fence.h/cpp`)
  - CPU/GPU 同步围栏类
  - 提供高效的等待机制
  - 支持围栏值管理和同步操作

- **DX12SwapChain** (`DX12SwapChain.h/cpp`)
  - 交换链管理类
  - 双缓冲支持
  - 窗口大小调整
  - 后台缓冲区管理

- **DX12DescriptorHeap** (`DX12DescriptorHeap.h/cpp`)
  - 描述符堆封装类
  - 支持 RTV、DSV、CBV/SRV/UAV 等类型
  - 提供 CPU/GPU 句柄访问方法
  - 自动计算描述符偏移


## 开发环境

- **IDE**: Visual Studio 2022
- **语言**: C++17
- **图形API**: DirectX 12
- **Windows SDK**: 10.0
- **平台**: Windows x64



## 学习参考

- 《Introduction to 3D Game Programming with DirectX 12》- Frank D. Luna
- DirectX 12 官方文档

## 许可证

MIT License
