#pragma once
#include <d3d12.h>
#include <vector>
#include <string>


//这里的设计想借鉴OGRE的模式：分离声明和绑定，主要是还要支持多种顶点类型



struct VertexElement
{
	std::string SemanticName;  //各种可能的组成语义position,color,normal等等
	UINT SemanticIndex;
	DXGI_FORMAT Format;
	UINT Offset;
	UINT Slot;

	VertexElement()
		: SemanticIndex(0)
		, Format(DXGI_FORMAT_UNKNOWN)
		, Offset(0)
		, Slot(0)
	{
	}
};



class VertexLayout
{
public:
	VertexLayout();
	~VertexLayout();

	// ====元素管理的相关的
	void AddElement(
		const std::string& semanticName,
		DXGI_FORMAT format,
		UINT offset,
		UINT semanticIndex = 0,
		UINT slot = 0
	);


	UINT GetElementCount() const { return static_cast<UINT>(m_elements.size()); }

	const VertexElement& GetElement(UINT index) const;


	//步长管理
	void CalculateStride();

	void SetStride(UINT stride) { m_stride = stride; }

	UINT GetStride() const { return m_stride; }




private:
	std::vector<VertexElement> m_elements;
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_d3d12Layout;
	UINT m_stride = 0;


	void UpdateD3D12Layout();

	static UINT GetFormatSize(DXGI_FORMAT format);
};