// D3D12VertexLayoutConverter.cpp
#include "Renderer/Resources/D3D12VertexLayoutConverter.h"
#include "Renderer/Resources/VertexFormat.h"


std::vector<D3D12_INPUT_ELEMENT_DESC> D3D12VertexLayoutConverter::Convert(const VertexLayout& layout)
{
    std::vector<D3D12_INPUT_ELEMENT_DESC> d3d12Elements;
    d3d12Elements.reserve(layout.GetElementCount());

    for (uint32_t i = 0; i < layout.GetElementCount(); ++i)
    {
        const VertexElement& element = layout.GetElement(i);

        D3D12_INPUT_ELEMENT_DESC desc = {};
        desc.SemanticName = element.SemanticName.c_str();
        desc.SemanticIndex = element.SemanticIndex;
        desc.Format = ConvertFormat(element.Format);
        desc.InputSlot = element.Slot;
        desc.AlignedByteOffset = element.Offset;
        desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        desc.InstanceDataStepRate = 0;

        d3d12Elements.push_back(desc);
    }

    return d3d12Elements;
}

DXGI_FORMAT D3D12VertexLayoutConverter::ConvertFormat(VertexFormat format)
{
    switch (format)
    {
        // Float类型
    case VertexFormat::Float1: return DXGI_FORMAT_R32_FLOAT;
    case VertexFormat::Float2: return DXGI_FORMAT_R32G32_FLOAT;
    case VertexFormat::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
    case VertexFormat::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;

        // UInt类型
    case VertexFormat::UInt1: return DXGI_FORMAT_R32_UINT;
    case VertexFormat::UInt2: return DXGI_FORMAT_R32G32_UINT;
    case VertexFormat::UInt3: return DXGI_FORMAT_R32G32B32_UINT;
    case VertexFormat::UInt4: return DXGI_FORMAT_R32G32B32A32_UINT;

        // Int类型
    case VertexFormat::Int1: return DXGI_FORMAT_R32_SINT;
    case VertexFormat::Int2: return DXGI_FORMAT_R32G32_SINT;
    case VertexFormat::Int3: return DXGI_FORMAT_R32G32B32_SINT;
    case VertexFormat::Int4: return DXGI_FORMAT_R32G32B32A32_SINT;

        // Half类型
    case VertexFormat::Half2: return DXGI_FORMAT_R16G16_FLOAT;
    case VertexFormat::Half4: return DXGI_FORMAT_R16G16B16A16_FLOAT;

        // 特殊格式
    case VertexFormat::UByte4_Norm: return DXGI_FORMAT_R8G8B8A8_UNORM;
    case VertexFormat::UByte4: return DXGI_FORMAT_R8G8B8A8_UINT;
    case VertexFormat::Short2_Norm: return DXGI_FORMAT_R16G16_SNORM;
    case VertexFormat::Short4_Norm: return DXGI_FORMAT_R16G16B16A16_SNORM;

    default: return DXGI_FORMAT_UNKNOWN;
    }
}

uint32_t D3D12VertexLayoutConverter::GetFormatSize(DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return 16;

    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        return 12;

    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
        return 8;

    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UINT:
        return 4;

    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SINT:
        return 2;

    default: return 0;
    }
}