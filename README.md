# KaiJing开境引擎 - DirectX 12 

基于《Introduction to 3D Game Programming with DirectX 12》，非原书代码，自己重新按照书上的流程重新封装

## 已完成
> 此为AI辅助总结的，可能不全

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
  - 提供 `CreateRTVs` 方法为所有后台缓冲区创建渲染目标视图
  - 提供 `GetCurrentRTVHandle` 和 `GetRTVHandle` 方法获取RTV句柄
  - 集成 `DX12DescriptorHeap` 进行RTV管理

- **DX12DescriptorHeap** (`DX12DescriptorHeap.h/cpp`)
  - 描述符堆封装类
  - 支持 RTV、DSV、CBV/SRV/UAV 等类型
  - 提供 CPU/GPU 句柄访问方法
  - 自动计算描述符偏移

- **DX12DepthStencilBuffer** (`DX12DepthStencilBuffer.h/cpp`)
  - 深度/模板缓冲区管理类
  - 创建和管理深度/模板缓冲区资源
  - 支持自定义格式和MSAA采样
  - 提供 `CreateDSV` 方法创建深度/模板视图
  - 支持窗口大小改变时自动调整缓冲区大小
  - 集成 `DX12Device` 单例和 `DX12DescriptorHeap` 类

- **DX12Viewport** (`DX12Viewport.h/cpp`)
  - 视口和裁剪矩形数据管理类
  - 封装 `D3D12_VIEWPORT` 和 `D3D12_RECT` 结构
  - 提供 `SetViewport` 和 `SetScissorRect` 方法设置参数
  - 提供 `SetDefault` 方法从交换链自动获取尺寸
  - 提供 `Resize` 方法支持窗口大小改变时调整
  - 提供 `SyncScissorToViewport` 方法自动同步裁剪矩形
  - 不依赖 Device 类，便于多窗口和多视口扩展

- **DX12ViewportUtils** (`DX12ViewportUtils.h/cpp`)
  - 视口工具函数命名空间
  - 提供 `Apply` 方法将视口和裁剪矩形应用到命令列表
  - 支持单个视口和多视口场景
  - 提供底层 D3D12 结构的 Apply 重载
  - 设计时考虑多窗口架构，不依赖 Device 单例



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
