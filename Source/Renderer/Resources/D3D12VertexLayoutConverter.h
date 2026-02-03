// D3D12VertexLayoutConverter.h
#pragma once
#include "Renderer/Resources/VertexLayout.h"
#include <d3d12.h>
#include <vector>

/**
 * @brief D3D12顶点布局转换器
 * @details 将API无关的VertexLayout转换为D3D12特定的格式
 */
class D3D12VertexLayoutConverter
{
public:
    /**
     * @brief 将VertexLayout转换为D3D12_INPUT_ELEMENT_DESC数组
     */
    static std::vector<D3D12_INPUT_ELEMENT_DESC> Convert(const VertexLayout& layout);

    /**
     * @brief 将VertexFormat转换为DXGI_FORMAT
     */
    static DXGI_FORMAT ConvertFormat(VertexFormat format);

    /**
     * @brief 获取格式的字节大小（D3D12）
     */
    static uint32_t GetFormatSize(DXGI_FORMAT format);

private:
    D3D12VertexLayoutConverter() = delete;  // 纯静态类
};