// VertexLayout.cpp
#include "Renderer/Resources/VertexLayout.h"
#include "Renderer/Resources/VertexFormat.h"

VertexLayout::VertexLayout() : m_stride(0)
{
}

VertexLayout::~VertexLayout()
{
    Clear();
}

void VertexLayout::AddElement(
    const std::string& semanticName,
    VertexFormat format,
    uint32_t offset,
    uint32_t semanticIndex,
    uint32_t slot)
{
    VertexElement element;
    element.SemanticName = semanticName;
    element.SemanticIndex = semanticIndex;
    element.Format = format;
    element.Offset = offset;
    element.Slot = slot;

    m_elements.push_back(element);
}

const VertexElement& VertexLayout::GetElement(uint32_t index) const
{
    if (index >= m_elements.size())
    {
        static const VertexElement empty;
        return empty;
    }
    return m_elements[index];
}

void VertexLayout::CalculateStride()
{
    if (m_elements.empty())
    {
        m_stride = 0;
        return;
    }

    uint32_t maxEndOffset = 0;

    for (const auto& element : m_elements)
    {
        uint32_t elementSize = GetVertexFormatSize(element.Format);
        uint32_t elementEnd = element.Offset + elementSize;

        if (elementEnd > maxEndOffset)
        {
            maxEndOffset = elementEnd;
        }
    }

    // 对齐到4字节边界
    m_stride = (maxEndOffset + 3) & ~3;
}

bool VertexLayout::operator==(const VertexLayout& other) const
{
    if (m_elements.size() != other.m_elements.size())
    {
        return false;
    }

    if (m_stride != other.m_stride)
    {
        return false;
    }

    for (size_t i = 0; i < m_elements.size(); ++i)
    {
        if (m_elements[i] != other.m_elements[i])
        {
            return false;
        }
    }

    return true;
}

void VertexLayout::Clear()
{
    m_elements.clear();
    m_stride = 0;
}