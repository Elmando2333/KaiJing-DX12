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

	//获取元素数量
	UINT GetElementCount() const { return static_cast<UINT>(m_elements.size()); }
	//获取元素
	const VertexElement& GetElement(UINT index) const;


	//步长管理
	void CalculateStride();

	void SetStride(UINT stride) { m_stride = stride; }

	UINT GetStride() const { return m_stride; }

	//d3d12的布局，来获取dx的需要的输入来创建PSO
	const D3D12_INPUT_ELEMENT_DESC* GetD3D12Layout() const;
	UINT GetD3D12LayoutSize() const { return static_cast<UINT>(m_d3d12Layout.size()); }


	//tool
	bool IsValid() const { return !m_elements.empty() && m_stride > 0; }

	void Clear();

	//比较布局
	bool operator==(const VertexLayout& other) const;
	bool operator!=(const VertexLayout& other) const { return !(*this == other); }



private:
	std::vector<VertexElement> m_elements;
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_d3d12Layout;
	UINT m_stride = 0;


	void UpdateD3D12Layout();

	static UINT GetFormatSize(DXGI_FORMAT format);
};