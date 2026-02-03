// VertexLayout.h
#pragma once
#include "Renderer/Resources/VertexFormat.h"
#include <vector>
#include <string>
#include <cstdint>

/**
 * @brief 顶点元素描述 - API无关
 */
struct VertexElement
{
    std::string SemanticName;    // 语义名称（如"POSITION", "NORMAL"）
    uint32_t SemanticIndex;      // 语义索引（如TEXCOORD0, TEXCOORD1）
    VertexFormat Format;          // 格式（API无关的枚举）
    uint32_t Offset;              // 在顶点中的偏移量
    uint32_t Slot;                // 输入槽索引（通常为0）

    VertexElement()
        : SemanticIndex(0)
        , Format(VertexFormat::Unknown)
        , Offset(0)
        , Slot(0)
    {
    }

    bool operator==(const VertexElement& other) const
    {
        return SemanticName == other.SemanticName &&
            SemanticIndex == other.SemanticIndex &&
            Format == other.Format &&
            Offset == other.Offset &&
            Slot == other.Slot;
    }
};

/**
 * @brief 顶点布局 - API无关的布局描述
 * @details 只描述顶点的结构，不包含任何渲染API特定信息
 */
class VertexLayout
{
public:
    VertexLayout();
    ~VertexLayout();

    /**
     * @brief 添加顶点元素
     */
    void AddElement(
        const std::string& semanticName,
        VertexFormat format,
        uint32_t offset,
        uint32_t semanticIndex = 0,
        uint32_t slot = 0
    );

    /**
     * @brief 获取元素数量
     */
    uint32_t GetElementCount() const { return static_cast<uint32_t>(m_elements.size()); }

    /**
     * @brief 获取指定索引的元素
     */
    const VertexElement& GetElement(uint32_t index) const;

    /**
     * @brief 计算并设置步长
     */
    void CalculateStride();

    /**
     * @brief 设置步长
     */
    void SetStride(uint32_t stride) { m_stride = stride; }

    /**
     * @brief 获取步长
     */
    uint32_t GetStride() const { return m_stride; }

    /**
     * @brief 检查布局是否有效
     */
    bool IsValid() const { return !m_elements.empty() && m_stride > 0; }

    /**
     * @brief 清空布局
     */
    void Clear();

    /**
     * @brief 比较操作符
     */
    bool operator==(const VertexLayout& other) const;
    bool operator!=(const VertexLayout& other) const { return !(*this == other); }

    /**
     * @brief 获取所有元素（只读）
     */
    const std::vector<VertexElement>& GetElements() const { return m_elements; }

private:
    std::vector<VertexElement> m_elements;
    uint32_t m_stride = 0;
};