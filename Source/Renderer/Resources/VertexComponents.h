// VertexComponents.h
#pragma once
#include "Renderer/Resources/VertexFormat.h"
#include <cstdint>

/**
 * @brief 顶点组件命名空间
 * @details 定义各种顶点组件，使用标准C++类型，完全API无关
 */
namespace VertexComponents
{
    /**
     * @brief 位置组件
     */
    struct Position
    {
        float x, y, z;

        static constexpr const char* SemanticName = "POSITION";
        static constexpr VertexFormat Format = VertexFormat::Float3;
        static constexpr uint32_t Size = sizeof(float) * 3;
    };

    /**
     * @brief 颜色组件
     */
    struct Color
    {
        float r, g, b, a;

        static constexpr const char* SemanticName = "COLOR";
        static constexpr VertexFormat Format = VertexFormat::Float4;
        static constexpr uint32_t Size = sizeof(float) * 4;
    };

    /**
     * @brief 法线组件
     */
    struct Normal
    {
        float x, y, z;

        static constexpr const char* SemanticName = "NORMAL";
        static constexpr VertexFormat Format = VertexFormat::Float3;
        static constexpr uint32_t Size = sizeof(float) * 3;
    };

    /**
     * @brief 纹理坐标组件
     */
    struct TexCoord
    {
        float u, v;

        static constexpr const char* SemanticName = "TEXCOORD";
        static constexpr VertexFormat Format = VertexFormat::Float2;
        static constexpr uint32_t Size = sizeof(float) * 2;
        static constexpr uint32_t SemanticIndex = 0;  // 可以支持多个UV
    };

    /**
     * @brief 第二套纹理坐标
     */
    struct TexCoord1
    {
        float u, v;

        static constexpr const char* SemanticName = "TEXCOORD";
        static constexpr VertexFormat Format = VertexFormat::Float2;
        static constexpr uint32_t Size = sizeof(float) * 2;
        static constexpr uint32_t SemanticIndex = 1;
    };

    /**
     * @brief 切线组件
     */
    struct Tangent
    {
        float x, y, z;

        static constexpr const char* SemanticName = "TANGENT";
        static constexpr VertexFormat Format = VertexFormat::Float3;
        static constexpr uint32_t Size = sizeof(float) * 3;
    };

    /**
     * @brief 副切线组件（双法线）
     */
    struct Binormal
    {
        float x, y, z;

        static constexpr const char* SemanticName = "BINORMAL";
        static constexpr VertexFormat Format = VertexFormat::Float3;
        static constexpr uint32_t Size = sizeof(float) * 3;
    };

    // TODO: 骨骼动画相关组件
    // struct BoneWeights { ... };
    // struct BoneIndices { ... };
}