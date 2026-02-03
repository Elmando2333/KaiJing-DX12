/*******************************************************************************

 * @file    DynamicVertexData.h
 * @brief   动态顶点数据管理器 - 运行时灵活的顶点格式处理
 * @author  Elmando2333
 * @date    2025-02-03
 * @version 1.0.0
 * @ingroup Graphics
 *
 * @remark  核心特性：
 *          1. 不依赖模板类型，运行时决定顶点格式
 *          2. 支持模型文件动态加载
 *          3. 材质热重载支持
 *          4. 自定义顶点格式配置
 *          5. GPU资源动态管理
 *
 * @note    从今天开始完善注释，提升代码可读性和可维护性
 *
 * @copyright MIT License
 
 ******************************************************************************/


#pragma once


//============================================================================
//                              INCLUDES
//============================================================================

#include "Renderer/Resources/VertexLayout.h"
#include <vector>
#include <cstdint>
#include <cstring>
#include <stdexcept>




class DynamicVertexData
{
public:

    //======================================================================
    //                       构造和析构/CONSTRUCTORS
    //======================================================================
    
    
    /**
     * @brief 空构造
     * @return 
     * @details 构造函数
     */
    DynamicVertexData();

    /**
     * @brief 从顶点和布局构造
     * @return
     * @details 构造函数
     */
    DynamicVertexData(const VertexLayout& layout, size_t vertexCount);


    
    /**
     * @brief 从布局和原始数据构造（深拷贝）
     * @return
     * @details 构造函数
     */
    DynamicVertexData(const VertexLayout& layout, const void* data, size_t vertexCount);

    
    /**
     * @brief 移动构造
     * @return
     * @details 构造函数
     */
    DynamicVertexData(DynamicVertexData&& other) noexcept;

    
    /**
     * @brief 移动赋值
     * @return
     * @details 构造函数
     */
    DynamicVertexData& operator=(DynamicVertexData&& other) noexcept;

    
    /**
     * @brief 析构（默认）
     * @return
     * @details 析构函数
     */
    ~DynamicVertexData() = default;


    //======================================================================
    //                       数据管理
    //======================================================================

    //重新分配
    void Resize(const VertexLayout& layout, size_t vertexCount);

    // 清空数据
    void Clear();

    // 检查是否有效
    bool IsValid() const { return m_layout.IsValid() && m_vertexCount > 0 && !m_data.empty(); }

private:
    // ========== 辅助函数 ==========

    // 查找元素（通过语义名称和索引）
    const VertexElement* FindElement(const std::string& semanticName, UINT semanticIndex) const;

    // 获取格式大小（复用 VertexLayout 的逻辑）
    static UINT GetFormatSize(DXGI_FORMAT format);

    
    //======================================================================
    //                       成员变量
    //======================================================================

    VertexLayout m_layout;           // 顶点布局
    std::vector<uint8_t> m_data;     // 原始字节数据
    size_t m_vertexCount;            // 顶点数量
};