// VertexFormat.h
#pragma once
#include <cstdint>

/**
 * @brief 顶点格式枚举 - API无关的格式定义
 */
enum class VertexFormat : uint8_t
{
    // Float类型
    Float1 = 0,
    Float2,
    Float3,
    Float4,
    
    // UInt类型
    UInt1,
    UInt2,
    UInt3,
    UInt4,
    
    // Int类型
    Int1,
    Int2,
    Int3,
    Int4,
    
    // Half类型（16位浮点）
    Half2,
    Half4,
    
    // 特殊格式
    UByte4_Norm,    // 4个无符号字节，归一化到[0,1]
    UByte4,         // 4个无符号字节
    Short2_Norm,    // 2个短整型，归一化
    Short4_Norm,    // 4个短整型，归一化
    
    Unknown = 255
};

/**
 * @brief 获取格式的字节大小
 */
inline uint32_t GetVertexFormatSize(VertexFormat format)
{
    switch (format)
    {
        case VertexFormat::Float1: return 4;
        case VertexFormat::Float2: return 8;
        case VertexFormat::Float3: return 12;
        case VertexFormat::Float4: return 16;
        
        case VertexFormat::UInt1: return 4;
        case VertexFormat::UInt2: return 8;
        case VertexFormat::UInt3: return 12;
        case VertexFormat::UInt4: return 16;
        
        case VertexFormat::Int1: return 4;
        case VertexFormat::Int2: return 8;
        case VertexFormat::Int3: return 12;
        case VertexFormat::Int4: return 16;
        
        case VertexFormat::Half2: return 4;
        case VertexFormat::Half4: return 8;
        
        case VertexFormat::UByte4_Norm:
        case VertexFormat::UByte4: return 4;
        case VertexFormat::Short2_Norm: return 4;
        case VertexFormat::Short4_Norm: return 8;
        
        default: return 0;
    }
}