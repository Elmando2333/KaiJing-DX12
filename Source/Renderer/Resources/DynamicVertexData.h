// DynamicVertexData.h
#pragma once
#include "Renderer/Resources/VertexLayout.h"
#include "Renderer/Resources/VertexFormat.h"
#include <vector>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>

/**
 * @brief 动态顶点数据管理器
 * @details 运行时灵活的顶点格式处理，支持动态添加、修改顶点数据
 */
class DynamicVertexData
{
public:
    // =======================================================================
    //                        构造和析构
    // =======================================================================

    /**
     * @brief 默认构造（空数据）
     */
    DynamicVertexData();

    /**
     * @brief 从布局和顶点数量构造（分配内存但不初始化数据）
     */
    DynamicVertexData(const VertexLayout& layout, size_t vertexCount);

    /**
     * @brief 从布局和原始数据构造（深拷贝）
     */
    DynamicVertexData(const VertexLayout& layout, const void* data, size_t vertexCount);

    /**
     * @brief 从模板顶点类型数组构造
     */
    template<typename VertexType>
    DynamicVertexData(const VertexType* vertices, size_t count);

    /**
     * @brief 移动构造
     */
    DynamicVertexData(DynamicVertexData&& other) noexcept;

    /**
     * @brief 移动赋值
     */
    DynamicVertexData& operator=(DynamicVertexData&& other) noexcept;

    /**
     * @brief 拷贝构造
     */
    DynamicVertexData(const DynamicVertexData& other);

    /**
     * @brief 拷贝赋值
     */
    DynamicVertexData& operator=(const DynamicVertexData& other);

    /**
     * @brief 析构函数
     */
    ~DynamicVertexData() = default;


    // =======================================================================
    //                        容量管理
    // =======================================================================

    /**
     * @brief 重新分配大小（会清空数据）
     */
    void Resize(const VertexLayout& layout, size_t vertexCount);

    /**
     * @brief 预留容量（避免频繁重分配）
     */
    void Reserve(size_t vertexCount);

    /**
     * @brief 获取当前容量（顶点数）
     */
    size_t GetCapacity() const;

    /**
     * @brief 收缩内存到实际大小
     */
    void ShrinkToFit();

    /**
     * @brief 清空所有数据
     */
    void Clear();


    // =======================================================================
    //                        查询接口
    // =======================================================================

    /**
     * @brief 检查数据是否有效
     */
    bool IsValid() const { return m_layout.IsValid() && m_vertexCount > 0 && !m_data.empty(); }

    /**
     * @brief 获取顶点数量
     */
    size_t GetVertexCount() const { return m_vertexCount; }

    /**
     * @brief 获取数据总大小（字节）
     */
    size_t GetDataSize() const { return m_data.size(); }

    /**
     * @brief 获取顶点步长（字节）
     */
    uint32_t GetStride() const { return m_layout.GetStride(); }

    /**
     * @brief 获取布局（只读）
     */
    const VertexLayout& GetLayout() const { return m_layout; }

    /**
     * @brief 检查顶点索引是否有效
     */
    bool IsValidVertexIndex(size_t index) const { return index < m_vertexCount; }

    /**
     * @brief 检查布局是否兼容（元素相同）
     */
    bool IsLayoutCompatible(const VertexLayout& other) const;


    // =======================================================================
    //                        数据访问
    // =======================================================================

    /**
     * @brief 获取原始数据指针（只读）
     */
    const void* GetData() const { return m_data.data(); }

    /**
     * @brief 获取原始数据指针（可写）
     */
    void* GetData() { return m_data.data(); }

    /**
     * @brief 获取指定顶点的数据指针（只读）
     */
    const void* GetVertexDataPtr(size_t index) const;

    /**
     * @brief 获取指定顶点的数据指针（可写）
     */
    void* GetVertexDataPtr(size_t index);


    // =======================================================================
    //                        顶点操作
    // =======================================================================

    /**
     * @brief 设置单个顶点的完整数据
     */
    void SetVertex(size_t index, const void* vertexData);

    /**
     * @brief 获取单个顶点的完整数据
     */
    void GetVertex(size_t index, void* outVertexData) const;

    /**
     * @brief 追加单个顶点
     */
    void AppendVertex(const void* vertexData);

    /**
     * @brief 追加多个顶点
     */
    void AppendVertices(const void* verticesData, size_t count);

    /**
     * @brief 在指定位置插入顶点
     */
    void InsertVertex(size_t index, const void* vertexData);

    /**
     * @brief 删除指定位置的顶点
     */
    void RemoveVertex(size_t index);

    /**
     * @brief 删除指定范围的顶点
     */
    void RemoveVertices(size_t startIndex, size_t count);


    // =======================================================================
    //                        属性访问（通用）
    // =======================================================================

    /**
     * @brief 设置顶点的特定属性
     */
    void SetVertexAttribute(size_t vertexIndex, const std::string& semanticName,
        uint32_t semanticIndex, const void* data);

    /**
     * @brief 获取顶点的特定属性
     */
    void GetVertexAttribute(size_t vertexIndex, const std::string& semanticName,
        uint32_t semanticIndex, void* outData) const;

    /**
     * @brief 类型安全的属性设置（模板版本）
     */
    template<typename T>
    void SetAttribute(size_t vertexIndex, const std::string& semanticName, const T& value);

    /**
     * @brief 类型安全的属性获取（模板版本）
     */
    template<typename T>
    T GetAttribute(size_t vertexIndex, const std::string& semanticName) const;


    // =======================================================================
    //                        便捷属性访问
    // =======================================================================

    /**
     * @brief 设置位置
     */
    void SetPosition(size_t index, float x, float y, float z);

    /**
     * @brief 获取位置
     */
    void GetPosition(size_t index, float& x, float& y, float& z) const;

    /**
     * @brief 设置颜色
     */
    void SetColor(size_t index, float r, float g, float b, float a);

    /**
     * @brief 获取颜色
     */
    void GetColor(size_t index, float& r, float& g, float& b, float& a) const;

    /**
     * @brief 设置法线
     */
    void SetNormal(size_t index, float x, float y, float z);

    /**
     * @brief 获取法线
     */
    void GetNormal(size_t index, float& x, float& y, float& z) const;

    /**
     * @brief 设置纹理坐标
     */
    void SetTexCoord(size_t index, float u, float v, uint32_t uvIndex = 0);

    /**
     * @brief 获取纹理坐标
     */
    void GetTexCoord(size_t index, float& u, float& v, uint32_t uvIndex = 0) const;

    /**
     * @brief 设置切线
     */
    void SetTangent(size_t index, float x, float y, float z);

    /**
     * @brief 获取切线
     */
    void GetTangent(size_t index, float& x, float& y, float& z) const;


    // =======================================================================
    //                        批量操作
    // =======================================================================

    /**
     * @brief 设置指定范围的顶点数据
     */
    void SetDataRange(size_t startVertex, size_t count, const void* data);

    /**
     * @brief 从另一个DynamicVertexData拷贝数据（布局必须兼容）
     */
    void CopyFrom(const DynamicVertexData& other, size_t srcStart,
        size_t dstStart, size_t count);


    // =======================================================================
    //                        模板类型转换
    // =======================================================================

    /**
     * @brief 从模板顶点类型数组创建DynamicVertexData
     */
    template<typename VertexType>
    static DynamicVertexData FromVertexArray(const VertexType* vertices, size_t count);

    /**
     * @brief 转换为模板顶点类型数组
     */
    template<typename VertexType>
    std::vector<VertexType> ToVertexArray() const;

private:
    // =======================================================================
    //                        辅助函数
    // =======================================================================

    /**
     * @brief 查找顶点元素
     */
    const VertexElement* FindElement(const std::string& semanticName, uint32_t semanticIndex) const;

    /**
     * @brief 获取属性数据指针（可写）
     */
    void* GetAttributePtr(size_t vertexIndex, const VertexElement& element);

    /**
     * @brief 获取属性数据指针（只读）
     */
    const void* GetAttributePtr(size_t vertexIndex, const VertexElement& element) const;

    /**
     * @brief 验证索引并抛出异常
     */
    void ValidateIndex(size_t index) const;

    /**
     * @brief 确保有足够容量
     */
    void EnsureCapacity(size_t requiredCount);


    // =======================================================================
    //                        成员变量
    // =======================================================================

    VertexLayout m_layout;           // 顶点布局
    std::vector<uint8_t> m_data;     // 原始字节数据
    size_t m_vertexCount = 0;        // 当前顶点数量
};