#include "DX12Viewport.h"
#include "DX12SwapChain.h"

DX12Viewport::DX12Viewport()
{
    m_viewport = {};
    m_scissorRect = {};
}

void DX12Viewport::SetViewport(float topLeftX, float topLeftY, float width, float height,
    float minDepth, float maxDepth)
{
    m_viewport.TopLeftX = topLeftX;
    m_viewport.TopLeftY = topLeftY;
    m_viewport.Width = width;
    m_viewport.Height = height;
    m_viewport.MinDepth = minDepth;
    m_viewport.MaxDepth = maxDepth;
}

void DX12Viewport::SetScissorRect(LONG left, LONG top, LONG right, LONG bottom)
{
    m_scissorRect.left = left;
    m_scissorRect.top = top;
    m_scissorRect.right = right;
    m_scissorRect.bottom = bottom;
}

void DX12Viewport::SetDefault(const DX12SwapChain& swapChain)
{
    if (!swapChain.IsInitialized())
    {
        return;
    }
    SetDefault(swapChain.GetWidth(), swapChain.GetHeight());
}

void DX12Viewport::SetDefault(UINT width, UINT height)
{
    SetViewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
    SetScissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height));
}

void DX12Viewport::Resize(float width, float height)
{
    m_viewport.Width = width;
    m_viewport.Height = height;
    SyncScissorToViewport();
}

void DX12Viewport::SyncScissorToViewport()
{
    m_scissorRect.left = static_cast<LONG>(m_viewport.TopLeftX);
    m_scissorRect.top = static_cast<LONG>(m_viewport.TopLeftY);
    m_scissorRect.right = static_cast<LONG>(m_viewport.TopLeftX + m_viewport.Width);
    m_scissorRect.bottom = static_cast<LONG>(m_viewport.TopLeftY + m_viewport.Height);
}