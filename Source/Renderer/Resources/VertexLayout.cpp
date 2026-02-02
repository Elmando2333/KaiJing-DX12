#include "VertexLayout.h"

VertexLayout::VertexLayout() :m_stride(0)
{
}

VertexLayout::~VertexLayout()
{
	Clear();
}

void VertexLayout::AddElement(const std::string& semanticName, DXGI_FORMAT format, UINT offset, UINT semanticIndex, UINT slot)
{
	VertexElement element;
	element.SemanticName = semanticName;
	element.SemanticIndex = semanticIndex;
	element.Format = format;
	element.Offset = offset;
	element.Slot = slot;

	m_elements.push_back(element);
	UpdateD3D12Layout();
}

const VertexElement& VertexLayout::GetElement(UINT index) const
{
	// TODO: 在此处插入 return 语句
	if (index >= m_elements.size())
	{
		static const VertexElement empty;//这个empty就是默认的
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

	UINT maxEndOffset = 0;

	for (const auto& element : m_elements)
	{
		UINT elementSize = GetFormatSize(element.Format);
		UINT elementEnd = element.Offset + elementSize;

		if (elementEnd > maxEndOffset)
		{
			maxEndOffset = elementEnd;
		}

	}
	m_stride = (maxEndOffset + 3) & ~3;
}

const D3D12_INPUT_ELEMENT_DESC* VertexLayout::GetD3D12Layout() const
{
	if (m_d3d12Layout.empty())
	{
		return nullptr;
	}
	return m_d3d12Layout.data();
}

bool VertexLayout::operator==(const VertexLayout& other) const
{
	if (m_elements.size() != other.GetElementCount())
	{
		return false;
	}
	if (m_stride != other.m_stride)
	{
		return false;
	}

	for (size_t i = 0; i < m_elements.size(); ++i)
	{
		const VertexElement& e1 = m_elements[i];
		const VertexElement& e2 = other.m_elements[i];

		if (e1.SemanticName != e2.SemanticName ||
			e1.SemanticIndex != e2.SemanticIndex ||
			e1.Format != e2.Format ||
			e1.Offset != e2.Offset ||
			e1.Slot != e2.Slot)
		{
			return false;
		}
	}
	return true;
}

void VertexLayout::UpdateD3D12Layout()
{
	m_d3d12Layout.clear();
	m_d3d12Layout.reserve(m_elements.size());


	for (const auto& element : m_elements)
	{
		D3D12_INPUT_ELEMENT_DESC desc = {};
		desc.SemanticName = element.SemanticName.c_str();
		desc.SemanticIndex = element.SemanticIndex;
		desc.Format = element.Format;
		desc.InputSlot = element.Slot;
		desc.AlignedByteOffset = element.Offset;
		desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		desc.InstanceDataStepRate = 0;
		
		m_d3d12Layout.push_back(desc);
	}
}

UINT VertexLayout::GetFormatSize(DXGI_FORMAT format)
{
	switch (format)
	{
		//32f
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return 16;
		case DXGI_FORMAT_R32G32B32_FLOAT:
			return 12;
		case DXGI_FORMAT_R32G32_FLOAT:
			return 8;
		case DXGI_FORMAT_R32_FLOAT:
			return 4;

		//16f
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			return 8;
		case DXGI_FORMAT_R16G16_FLOAT:
			return 4;
		case DXGI_FORMAT_R16_FLOAT:
			return 2;

		//32i
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
			return 16;
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT:
			return 12;
		case DXGI_FORMAT_R32G32_UINT:
		case DXGI_FORMAT_R32G32_SINT:
			return 8;
		case DXGI_FORMAT_R32_UINT:
		case DXGI_FORMAT_R32_SINT:
			return 4;


		//16i
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SINT:
			return 8;
		case DXGI_FORMAT_R16G16_UINT:
		case DXGI_FORMAT_R16G16_SINT:
			return 4;
		case DXGI_FORMAT_R16_UINT:
		case DXGI_FORMAT_R16_SINT:
			return 2;
		
		//8i
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SINT:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
			return 4;
		case DXGI_FORMAT_R8G8_UINT:
		case DXGI_FORMAT_R8G8_SINT:
		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R8G8_SNORM:
			return 2;
		case DXGI_FORMAT_R8_UINT:
		case DXGI_FORMAT_R8_SINT:
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R8_SNORM:
			return 1;

		//其他
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_R10G10B10A2_UNORM:
			return 4;
		case DXGI_FORMAT_R11G11B10_FLOAT:
			return 4;


		//默认
		default:
			return 4;

	}
	return 0;
}


void VertexLayout::Clear()
{
	m_elements.clear();
	m_d3d12Layout.clear();
	m_stride = 0;

}
