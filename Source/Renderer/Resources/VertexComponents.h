#pragma once
#include <DirectXMath.h>
#include <d3d12.h>


//这个是模拟仿照UE的顶点风格写的，同时希望后面用模板来减少代码重复的问题



namespace VertexComponents
{
	// =======基础的组件（位置，颜色，法向量等等之类的）



	//位置
	struct Position
	{
		DirectX::XMFLOAT3 Value;

		static constexpr const char* SemanticName = "POSITION";
		static constexpr DXGI_FORMAT Format = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr UINT Size = sizeof(DirectX::XMFLOAT3);
	};

	//颜色
	struct Color
	{
		DirectX::XMFLOAT4 Value;

		static constexpr const char* SemanticName = "COLOR";
		static constexpr DXGI_FORMAT Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		static constexpr UINT Size = sizeof(DirectX::XMFLOAT4);
	};


	//法线
	struct Normal
	{
		DirectX::XMFLOAT3 Value;

		static constexpr const char* SemanticName = "NORMAL";
		static constexpr DXGI_FORMAT Format = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr UINT Size = sizeof(DirectX::XMFLOAT3);
	};


	//纹理坐标
	struct TexCoord
	{
		DirectX::XMFLOAT2 Value;

		static constexpr const char* SemanticName = "TEXCOORD";
		static constexpr DXGI_FORMAT Format = DXGI_FORMAT_R32G32_FLOAT;
		static constexpr UINT Size = sizeof(DirectX::XMFLOAT2); 
	};

	
	//切线
	struct Tangent
	{
		DirectX::XMFLOAT3 Value;

		static constexpr const char* SemanticName = "TANGENT";
		static constexpr DXGI_FORMAT Format = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr UINT Size = sizeof(DirectX::XMFLOAT3);
	};

	//副的切线，双发现，法线贴图的
	struct Binormal
	{
		DirectX::XMFLOAT3 Value;

		static constexpr const char* SemanticName = "BINORMAL";
		static constexpr DXGI_FORMAT Format = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr UINT Size = sizeof(DirectX::XMFLOAT3);
	};


	//==========牛b的高级组件
	struct BoneWeights
	{
		//TODO：以后用到再写
	};

	struct BoneIndices
	{
		//TODO：以后用到再写
	};

	//====扩展的组件

	//第二套纹理
	struct TexCoord1
	{
		//TODO：以后用到再写
	};

	//实例数据，颜色，后面再说
}