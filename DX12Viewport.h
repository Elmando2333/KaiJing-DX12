#pragma once
#include <d3d12.h>
//本来想写到device里，但觉得后面多视口管理起来不方便，就单开一个类来管理视口数据


class DX12SwapChain;

//不依赖device,这个类只管理视口的数据
class DX12Viewport
{
public:
    DX12Viewport();
    ~DX12Viewport() = default;

    // 禁用
    DX12Viewport(const DX12Viewport&) = delete;
    DX12Viewport& operator=(const DX12Viewport&) = delete;

    // 设置视口参数
    void SetViewport(
        float topLeftX,
        float topLeftY,
        float width,
        float height,
        float minDepth = 0.0f,
        float maxDepth = 1.0f
    );

    // 设置一下裁剪矩形
    void SetScissorRect(
        LONG left,
        LONG top,
        LONG right,
        LONG bottom
    );

    // 设置默认视口和裁剪矩形，从交换链获取尺寸
    void SetDefault(const DX12SwapChain& swapChain);

    // 设置默认视口和裁剪矩形（上面是从交换链获取的，所以基本上就是整个窗口大小，这个是指定宽高）
    void SetDefault(UINT width, UINT height);

    void Resize(float width, float height);

    // 自动同步裁剪矩形到视口大小
    void SyncScissorToViewport();

    // 获取视口结构
    const D3D12_VIEWPORT& GetViewport() const { return m_viewport; }

    // 获取裁剪矩形结构
    const D3D12_RECT& GetScissorRect() const { return m_scissorRect; }


    // 视口各个属性
    float GetTopLeftX() const { return m_viewport.TopLeftX; }
    float GetTopLeftY() const { return m_viewport.TopLeftY; }
    float GetWidth() const { return m_viewport.Width; }
    float GetHeight() const { return m_viewport.Height; }
    float GetMinDepth() const { return m_viewport.MinDepth; }
    float GetMaxDepth() const { return m_viewport.MaxDepth; }

    bool IsValid() const
    {
        return m_viewport.Width > 0.0f && m_viewport.Height > 0.0f;
    }

private:
    D3D12_VIEWPORT m_viewport = {};
    D3D12_RECT m_scissorRect = {};

};

