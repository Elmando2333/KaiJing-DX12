// Vertex.h
#pragma once
#include "Renderer/Resources/VertexComponents.h"
#include "Renderer/Resources/VertexLayout.h"
#include <cstddef>

/**
 * @brief 顶点模板 - 使用组件组合定义顶点类型
 */
template<typename... Components>
struct SVertex;

// 特化1：单个组件
template<typename Component>
struct SVertex<Component>
{
    Component component;

    static VertexLayout GetLayout()
    {
        VertexLayout layout;
        layout.AddElement(
            Component::SemanticName,
            Component::Format,
            offsetof(SVertex, component),
            Component::SemanticIndex
        );
        layout.CalculateStride();
        return layout;
    }

    static constexpr uint32_t GetStride()
    {
        return sizeof(SVertex);
    }
};

// 特化2：多个组件递归
template<typename First, typename... Rest>
struct SVertex<First, Rest...>
{
    First first;
    SVertex<Rest...> rest;

    static VertexLayout GetLayout()
    {
        VertexLayout layout;
        BuildLayoutRecursive<First, Rest...>(layout, 0);
        layout.CalculateStride();
        return layout;
    }

    static constexpr uint32_t GetStride()
    {
        return sizeof(SVertex);
    }

private:
    template<typename T>
    static void BuildLayoutRecursive(VertexLayout& layout, uint32_t currentOffset)
    {
        uint32_t semanticIndex = 0;
        if constexpr (requires { T::SemanticIndex; })
        {
            semanticIndex = T::SemanticIndex;
        }

        layout.AddElement(
            T::SemanticName,
            T::Format,
            currentOffset,
            semanticIndex
        );
    }

    template<typename T, typename U, typename... Args>
    static void BuildLayoutRecursive(VertexLayout& layout, uint32_t currentOffset)
    {
        uint32_t semanticIndex = 0;
        if constexpr (requires { T::SemanticIndex; })
        {
            semanticIndex = T::SemanticIndex;
        }

        layout.AddElement(
            T::SemanticName,
            T::Format,
            currentOffset,
            semanticIndex
        );

        // 递归处理下一个组件
        BuildLayoutRecursive<U, Args...>(layout, currentOffset + T::Size);
    }
};

// 常用顶点类型定义
using SPositionColorVertex = SVertex<
    VertexComponents::Position,
    VertexComponents::Color
>;

using SPositionNormalTexVertex = SVertex<
    VertexComponents::Position,
    VertexComponents::Normal,
    VertexComponents::TexCoord
>;

using SPositionNormalTexTangentVertex = SVertex<
    VertexComponents::Position,
    VertexComponents::Normal,
    VertexComponents::TexCoord,
    VertexComponents::Tangent
>;

using SPositionNormalTexTangentBinormalVertex = SVertex<
    VertexComponents::Position,
    VertexComponents::Normal,
    VertexComponents::TexCoord,
    VertexComponents::Tangent,
    VertexComponents::Binormal
>;

using SPositionColorNormalTexVertex = SVertex<
    VertexComponents::Position,
    VertexComponents::Color,
    VertexComponents::Normal,
    VertexComponents::TexCoord
>;