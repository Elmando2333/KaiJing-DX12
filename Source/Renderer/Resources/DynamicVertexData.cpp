// DynamicVertexData.cpp
#include "Renderer/Resources/DynamicVertexData.h"
#include "Renderer/Resources/VertexFormat.h"
#include "Renderer/Resources/Vertex.h"
#include <algorithm>
#include <cassert>

// =======================================================================
//                        构造和析构
// =======================================================================

DynamicVertexData::DynamicVertexData()
    : m_vertexCount(0)
{
}

DynamicVertexData::DynamicVertexData(const VertexLayout& layout, size_t vertexCount)
    : m_layout(layout)
    , m_vertexCount(vertexCount)
{
    if (m_layout.IsValid() && vertexCount > 0)
    {
        size_t dataSize = m_layout.GetStride() * vertexCount;
        m_data.resize(dataSize);
    }
}

DynamicVertexData::DynamicVertexData(const VertexLayout& layout, const void* data, size_t vertexCount)
    : m_layout(layout)
    , m_vertexCount(vertexCount)
{
    if (m_layout.IsValid() && vertexCount > 0 && data)
    {
        size_t dataSize = m_layout.GetStride() * vertexCount;
        m_data.resize(dataSize);
        std::memcpy(m_data.data(), data, dataSize);
    }
}

template<typename VertexType>
DynamicVertexData::DynamicVertexData(const VertexType* vertices, size_t count)
    : m_layout(VertexType::GetLayout())
    , m_vertexCount(count)
{
    if (count > 0 && vertices)
    {
        size_t dataSize = m_layout.GetStride() * count;
        m_data.resize(dataSize);
        std::memcpy(m_data.data(), vertices, dataSize);
    }
}

DynamicVertexData::DynamicVertexData(DynamicVertexData&& other) noexcept
    : m_layout(std::move(other.m_layout))
    , m_data(std::move(other.m_data))
    , m_vertexCount(other.m_vertexCount)
{
    other.m_vertexCount = 0;
}

DynamicVertexData& DynamicVertexData::operator=(DynamicVertexData&& other) noexcept
{
    if (this != &other)
    {
        m_layout = std::move(other.m_layout);
        m_data = std::move(other.m_data);
        m_vertexCount = other.m_vertexCount;
        other.m_vertexCount = 0;
    }
    return *this;
}

DynamicVertexData::DynamicVertexData(const DynamicVertexData& other)
    : m_layout(other.m_layout)
    , m_data(other.m_data)
    , m_vertexCount(other.m_vertexCount)
{
}

DynamicVertexData& DynamicVertexData::operator=(const DynamicVertexData& other)
{
    if (this != &other)
    {
        m_layout = other.m_layout;
        m_data = other.m_data;
        m_vertexCount = other.m_vertexCount;
    }
    return *this;
}


// =======================================================================
//                        容量管理
// =======================================================================

void DynamicVertexData::Resize(const VertexLayout& layout, size_t vertexCount)
{
    m_layout = layout;
    m_vertexCount = vertexCount;

    if (m_layout.IsValid() && vertexCount > 0)
    {
        size_t dataSize = m_layout.GetStride() * vertexCount;
        m_data.resize(dataSize);
    }
    else
    {
        m_data.clear();
    }
}

void DynamicVertexData::Reserve(size_t vertexCount)
{
    if (vertexCount > 0 && m_layout.IsValid())
    {
        size_t dataSize = m_layout.GetStride() * vertexCount;
        m_data.reserve(dataSize);
    }
}

size_t DynamicVertexData::GetCapacity() const
{
    if (!m_layout.IsValid() || m_layout.GetStride() == 0)
    {
        return 0;
    }
    return m_data.capacity() / m_layout.GetStride();
}

void DynamicVertexData::ShrinkToFit()
{
    if (m_layout.IsValid() && m_vertexCount > 0)
    {
        size_t requiredSize = m_layout.GetStride() * m_vertexCount;
        if (m_data.size() > requiredSize)
        {
            m_data.resize(requiredSize);
        }
    }
    m_data.shrink_to_fit();
}

void DynamicVertexData::Clear()
{
    m_layout.Clear();
    m_data.clear();
    m_vertexCount = 0;
}


// =======================================================================
//                        查询接口
// =======================================================================

bool DynamicVertexData::IsLayoutCompatible(const VertexLayout& other) const
{
    return m_layout == other;
}


// =======================================================================
//                        数据访问
// =======================================================================

const void* DynamicVertexData::GetVertexDataPtr(size_t index) const
{
    ValidateIndex(index);
    uint32_t stride = m_layout.GetStride();
    return m_data.data() + (index * stride);
}

void* DynamicVertexData::GetVertexDataPtr(size_t index)
{
    ValidateIndex(index);
    uint32_t stride = m_layout.GetStride();
    return m_data.data() + (index * stride);
}


// =======================================================================
//                        顶点操作
// =======================================================================

void DynamicVertexData::SetVertex(size_t index, const void* vertexData)
{
    ValidateIndex(index);
    if (!vertexData)
    {
        throw std::invalid_argument("vertexData cannot be null");
    }

    uint32_t stride = m_layout.GetStride();
    void* dst = m_data.data() + (index * stride);
    std::memcpy(dst, vertexData, stride);
}

void DynamicVertexData::GetVertex(size_t index, void* outVertexData) const
{
    ValidateIndex(index);
    if (!outVertexData)
    {
        throw std::invalid_argument("outVertexData cannot be null");
    }

    uint32_t stride = m_layout.GetStride();
    const void* src = m_data.data() + (index * stride);
    std::memcpy(outVertexData, src, stride);
}

void DynamicVertexData::AppendVertex(const void* vertexData)
{
    if (!m_layout.IsValid())
    {
        throw std::runtime_error("Cannot append vertex: layout is invalid");
    }

    EnsureCapacity(m_vertexCount + 1);

    uint32_t stride = m_layout.GetStride();
    size_t newSize = (m_vertexCount + 1) * stride;
    m_data.resize(newSize);

    if (vertexData)
    {
        void* dst = m_data.data() + (m_vertexCount * stride);
        std::memcpy(dst, vertexData, stride);
    }

    ++m_vertexCount;
}

void DynamicVertexData::AppendVertices(const void* verticesData, size_t count)
{
    if (!m_layout.IsValid())
    {
        throw std::runtime_error("Cannot append vertices: layout is invalid");
    }

    if (count == 0)
    {
        return;
    }

    EnsureCapacity(m_vertexCount + count);

    uint32_t stride = m_layout.GetStride();
    size_t oldSize = m_vertexCount * stride;
    size_t newSize = (m_vertexCount + count) * stride;
    m_data.resize(newSize);

    if (verticesData)
    {
        void* dst = m_data.data() + oldSize;
        std::memcpy(dst, verticesData, count * stride);
    }

    m_vertexCount += count;
}

void DynamicVertexData::InsertVertex(size_t index, const void* vertexData)
{
    if (index > m_vertexCount)
    {
        throw std::out_of_range("Insert index out of range");
    }

    if (!m_layout.IsValid())
    {
        throw std::runtime_error("Cannot insert vertex: layout is invalid");
    }

    EnsureCapacity(m_vertexCount + 1);

    uint32_t stride = m_layout.GetStride();
    size_t insertPos = index * stride;
    size_t oldSize = m_vertexCount * stride;

    m_data.resize((m_vertexCount + 1) * stride);

    // 移动后面的数据
    if (index < m_vertexCount)
    {
        void* dst = m_data.data() + insertPos + stride;
        const void* src = m_data.data() + insertPos;
        std::memmove(dst, src, oldSize - insertPos);
    }

    // 插入新数据
    if (vertexData)
    {
        void* dst = m_data.data() + insertPos;
        std::memcpy(dst, vertexData, stride);
    }

    ++m_vertexCount;
}

void DynamicVertexData::RemoveVertex(size_t index)
{
    ValidateIndex(index);

    if (m_vertexCount == 1)
    {
        Clear();
        return;
    }

    uint32_t stride = m_layout.GetStride();
    size_t removePos = index * stride;
    size_t dataSize = m_vertexCount * stride;

    // 移动后面的数据
    if (index < m_vertexCount - 1)
    {
        void* dst = m_data.data() + removePos;
        const void* src = m_data.data() + removePos + stride;
        std::memmove(dst, src, dataSize - removePos - stride);
    }

    --m_vertexCount;
    m_data.resize(m_vertexCount * stride);
}

void DynamicVertexData::RemoveVertices(size_t startIndex, size_t count)
{
    if (startIndex >= m_vertexCount)
    {
        throw std::out_of_range("Remove start index out of range");
    }

    if (count == 0)
    {
        return;
    }

    size_t endIndex = startIndex + count;
    if (endIndex > m_vertexCount)
    {
        endIndex = m_vertexCount;
        count = endIndex - startIndex;
    }

    if (count == m_vertexCount)
    {
        Clear();
        return;
    }

    uint32_t stride = m_layout.GetStride();
    size_t removeStartPos = startIndex * stride;
    size_t removeEndPos = endIndex * stride;
    size_t dataSize = m_vertexCount * stride;

    // 移动后面的数据
    if (endIndex < m_vertexCount)
    {
        void* dst = m_data.data() + removeStartPos;
        const void* src = m_data.data() + removeEndPos;
        std::memmove(dst, src, dataSize - removeEndPos);
    }

    m_vertexCount -= count;
    m_data.resize(m_vertexCount * stride);
}


// =======================================================================
//                        属性访问（通用）
// =======================================================================

void DynamicVertexData::SetVertexAttribute(size_t vertexIndex, const std::string& semanticName,
    uint32_t semanticIndex, const void* data)
{
    ValidateIndex(vertexIndex);

    const VertexElement* element = FindElement(semanticName, semanticIndex);
    if (!element)
    {
        throw std::runtime_error("Vertex attribute not found: " + semanticName +
            "[" + std::to_string(semanticIndex) + "]");
    }

    if (!data)
    {
        throw std::invalid_argument("data cannot be null");
    }

    void* attrPtr = GetAttributePtr(vertexIndex, *element);
    uint32_t attrSize = GetVertexFormatSize(element->Format);
    std::memcpy(attrPtr, data, attrSize);
}

void DynamicVertexData::GetVertexAttribute(size_t vertexIndex, const std::string& semanticName,
    uint32_t semanticIndex, void* outData) const
{
    ValidateIndex(vertexIndex);

    const VertexElement* element = FindElement(semanticName, semanticIndex);
    if (!element)
    {
        throw std::runtime_error("Vertex attribute not found: " + semanticName +
            "[" + std::to_string(semanticIndex) + "]");
    }

    if (!outData)
    {
        throw std::invalid_argument("outData cannot be null");
    }

    const void* attrPtr = GetAttributePtr(vertexIndex, *element);
    uint32_t attrSize = GetVertexFormatSize(element->Format);
    std::memcpy(outData, attrPtr, attrSize);
}

template<typename T>
void DynamicVertexData::SetAttribute(size_t vertexIndex, const std::string& semanticName, const T& value)
{
    SetVertexAttribute(vertexIndex, semanticName, 0, &value);
}

template<typename T>
T DynamicVertexData::GetAttribute(size_t vertexIndex, const std::string& semanticName) const
{
    T value;
    GetVertexAttribute(vertexIndex, semanticName, 0, &value);
    return value;
}


// =======================================================================
//                        便捷属性访问
// =======================================================================

void DynamicVertexData::SetPosition(size_t index, float x, float y, float z)
{
    float pos[3] = { x, y, z };
    SetVertexAttribute(index, "POSITION", 0, pos);
}

void DynamicVertexData::GetPosition(size_t index, float& x, float& y, float& z) const
{
    float pos[3];
    GetVertexAttribute(index, "POSITION", 0, pos);
    x = pos[0];
    y = pos[1];
    z = pos[2];
}

void DynamicVertexData::SetColor(size_t index, float r, float g, float b, float a)
{
    float color[4] = { r, g, b, a };
    SetVertexAttribute(index, "COLOR", 0, color);
}

void DynamicVertexData::GetColor(size_t index, float& r, float& g, float& b, float& a) const
{
    float color[4];
    GetVertexAttribute(index, "COLOR", 0, color);
    r = color[0];
    g = color[1];
    b = color[2];
    a = color[3];
}

void DynamicVertexData::SetNormal(size_t index, float x, float y, float z)
{
    float normal[3] = { x, y, z };
    SetVertexAttribute(index, "NORMAL", 0, normal);
}

void DynamicVertexData::GetNormal(size_t index, float& x, float& y, float& z) const
{
    float normal[3];
    GetVertexAttribute(index, "NORMAL", 0, normal);
    x = normal[0];
    y = normal[1];
    z = normal[2];
}

void DynamicVertexData::SetTexCoord(size_t index, float u, float v, uint32_t uvIndex)
{
    float uv[2] = { u, v };
    SetVertexAttribute(index, "TEXCOORD", uvIndex, uv);
}

void DynamicVertexData::GetTexCoord(size_t index, float& u, float& v, uint32_t uvIndex) const
{
    float uv[2];
    GetVertexAttribute(index, "TEXCOORD", uvIndex, uv);
    u = uv[0];
    v = uv[1];
}

void DynamicVertexData::SetTangent(size_t index, float x, float y, float z)
{
    float tangent[3] = { x, y, z };
    SetVertexAttribute(index, "TANGENT", 0, tangent);
}

void DynamicVertexData::GetTangent(size_t index, float& x, float& y, float& z) const
{
    float tangent[3];
    GetVertexAttribute(index, "TANGENT", 0, tangent);
    x = tangent[0];
    y = tangent[1];
    z = tangent[2];
}


// =======================================================================
//                        批量操作
// =======================================================================

void DynamicVertexData::SetDataRange(size_t startVertex, size_t count, const void* data)
{
    if (startVertex + count > m_vertexCount)
    {
        throw std::out_of_range("Data range out of bounds");
    }

    if (!data)
    {
        throw std::invalid_argument("data cannot be null");
    }

    uint32_t stride = m_layout.GetStride();
    void* dst = m_data.data() + (startVertex * stride);
    std::memcpy(dst, data, count * stride);
}

void DynamicVertexData::CopyFrom(const DynamicVertexData& other, size_t srcStart,
    size_t dstStart, size_t count)
{
    if (!IsLayoutCompatible(other.m_layout))
    {
        throw std::runtime_error("Layouts are not compatible");
    }

    if (srcStart + count > other.m_vertexCount)
    {
        throw std::out_of_range("Source range out of bounds");
    }

    if (dstStart + count > m_vertexCount)
    {
        throw std::out_of_range("Destination range out of bounds");
    }

    uint32_t stride = m_layout.GetStride();
    const void* src = other.m_data.data() + (srcStart * stride);
    void* dst = m_data.data() + (dstStart * stride);
    std::memcpy(dst, src, count * stride);
}


// =======================================================================
//                        模板类型转换
// =======================================================================

template<typename VertexType>
DynamicVertexData DynamicVertexData::FromVertexArray(const VertexType* vertices, size_t count)
{
    return DynamicVertexData(vertices, count);
}

template<typename VertexType>
std::vector<VertexType> DynamicVertexData::ToVertexArray() const
{
    if (!m_layout.IsValid())
    {
        throw std::runtime_error("Cannot convert: layout is invalid");
    }

    // 检查布局是否匹配
    VertexLayout vertexLayout = VertexType::GetLayout();
    if (m_layout != vertexLayout)
    {
        throw std::runtime_error("Layout mismatch: cannot convert to VertexType");
    }

    std::vector<VertexType> result(m_vertexCount);
    uint32_t stride = m_layout.GetStride();

    for (size_t i = 0; i < m_vertexCount; ++i)
    {
        const void* src = m_data.data() + (i * stride);
        std::memcpy(&result[i], src, stride);
    }

    return result;
}


// =======================================================================
//                        辅助函数
// =======================================================================

const VertexElement* DynamicVertexData::FindElement(const std::string& semanticName, uint32_t semanticIndex) const
{
    for (uint32_t i = 0; i < m_layout.GetElementCount(); ++i)
    {
        const VertexElement& element = m_layout.GetElement(i);
        if (element.SemanticName == semanticName && element.SemanticIndex == semanticIndex)
        {
            return &element;
        }
    }
    return nullptr;
}

void* DynamicVertexData::GetAttributePtr(size_t vertexIndex, const VertexElement& element)
{
    uint32_t stride = m_layout.GetStride();
    uint8_t* vertexPtr = m_data.data() + (vertexIndex * stride);
    return vertexPtr + element.Offset;
}

const void* DynamicVertexData::GetAttributePtr(size_t vertexIndex, const VertexElement& element) const
{
    uint32_t stride = m_layout.GetStride();
    const uint8_t* vertexPtr = m_data.data() + (vertexIndex * stride);
    return vertexPtr + element.Offset;
}

void DynamicVertexData::ValidateIndex(size_t index) const
{
    if (index >= m_vertexCount)
    {
        throw std::out_of_range("Vertex index out of range: " + std::to_string(index) +
            " (count: " + std::to_string(m_vertexCount) + ")");
    }
}

void DynamicVertexData::EnsureCapacity(size_t requiredCount)
{
    if (requiredCount > GetCapacity())
    {
        size_t newCapacity = std::max(requiredCount, GetCapacity() * 2);
        Reserve(newCapacity);
    }
}