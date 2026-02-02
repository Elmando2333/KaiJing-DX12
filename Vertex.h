#pragma once
#include "Renderer/Resources/VertexComponents.h"
#include "Renderer/Resources/VertexLayout.h"
#include <cstddef>

//我要革命！UE把结构体前缀竟然写成F，我就要改成S！

template<typename... Components>
struct SVertex;


//先特化

//特化1：特化单个组件
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
			offsetof(SVertex, component)
		);
		layout.CalculateStride();
		return layout;
	}

	static constexpr UINT GetStride()
	{
		return sizeof(SVertex);
	}
};




//特化2，多个组件递归
template<typename First,typename... Rest>
struct SVertex<First, Rest...>
{
	First first;
	SVertex<Rest...> rest;


	//获取布局
	static VertexLayout GetLayout()
	{
		VertexLayout layout;
		BuildLayoutRecursive<First, Rest...>(layout, 0);
		layout.CalculateStride();
		return layout;
	}

	//顶点步长
	static constexpr UINT GetStride()
	{
		return sizeof(SVertex);
	}

private:
	template<typename T>
	static void BuildLayoutRecursive(VertexLayout& layout, UINT currentOffset)
	{
		//计算当前组件在结构体中的实际偏移
		SVertex<T> temp;
		UINT offset = static_cast<UINT>(reinterpret_cast<const char*>(&temp.component) -
			reinterpret_cast<const char*>(&temp));


		UINT semanticIndex = 0;
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
	static void BuildLayoutRecursive(VertexLayout& layout, UINT currentOffset)
	{
		// 添加当前组件
		UINT semanticIndex = 0;
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

		// 递归处理
		BuildLayoutRecursive<U, Args...>(layout, currentOffset + T::Size);
	}



	//写些常用顶点类型：
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

};
