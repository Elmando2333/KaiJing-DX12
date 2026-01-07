#pragma once
#include <d3d12.h>

class DX12Viewport;

//这里是视口处理的一些工具函数
namespace DX12ViewportUtils
{
    // 应用视口和裁剪矩形到命令列表
    void Apply(ID3D12GraphicsCommandList* cmdList, const DX12Viewport& viewport);

    // 应用多个视口和裁剪矩形
    void Apply(ID3D12GraphicsCommandList* cmdList, const DX12Viewport* viewports, UINT count);

    
    void Apply(ID3D12GraphicsCommandList* cmdList,
        const D3D12_VIEWPORT& viewport,
        const D3D12_RECT& scissorRect);
}

