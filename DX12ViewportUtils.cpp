#include "DX12ViewportUtils.h"
#include "DX12Viewport.h"
#include <vector>


namespace DX12ViewportUtils 
{
    //一个视口的
    void Apply(ID3D12GraphicsCommandList* cmdList, const DX12Viewport& viewport)
    {
        if (!cmdList || !viewport.IsValid())
        {
            return;
        }

        cmdList->RSSetViewports(1, &viewport.GetViewport());
        cmdList->RSSetScissorRects(1, &viewport.GetScissorRect());
    }

    //多个视口
    void Apply(ID3D12GraphicsCommandList* cmdList, const DX12Viewport* viewports, UINT count)
    {
        if (!cmdList || !viewports || count == 0)
        {
            return;
        }

        // 转换一下
        std::vector<D3D12_VIEWPORT> d3dViewports(count);
        std::vector<D3D12_RECT> d3dRects(count);

        for (UINT i = 0; i < count; i++)
        {
            d3dViewports[i] = viewports[i].GetViewport();
            d3dRects[i] = viewports[i].GetScissorRect();
        }

        cmdList->RSSetViewports(count, d3dViewports.data());
        cmdList->RSSetScissorRects(count, d3dRects.data());
    }

    //这是最原始的直接创建方法
    void Apply(ID3D12GraphicsCommandList* cmdList,
        const D3D12_VIEWPORT& viewport,
        const D3D12_RECT& scissorRect)
    {
        if (!cmdList)
        {
            return;
        }

        cmdList->RSSetViewports(1, &viewport);
        cmdList->RSSetScissorRects(1, &scissorRect);
    }
}