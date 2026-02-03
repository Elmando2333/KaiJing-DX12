#pragma once
#include "Renderer/Resources/Vertex.h"
#include "Renderer/Resources/VertexLayout.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <typeinfo>



//顶点工厂
//这个统一管理不同类型的顶点
//编译器去为每一个顶点类型去生成一个类，解放自我！
// 现学现卖，CRTP,让基类定义接口，模板派生类提供具体实现，让每个类型有自己的特化版本
//TODO: 查一下函数用法注释的写法

class SVertexFactoryBase
{
public:
	virtual ~SVertexFactoryBase() = default;

	//获取顶点布局
	virtual VertexLayout GetLayout() const = 0;

	//获取顶点步长
	virtual UINT GetStride() const = 0;

	//获取顶点类型名称
	virtual const char* GetTypeName() const = 0;

	//获取类型ID
	virtual const char* GetTypeId() const = 0;

};


//下面的实现是模板工厂，主要目的是为任何顶点类型创建工厂，减少代码重复
template<typename VertexType>
class SVertexFactory : public SVertexFactoryBase
{
public:

	//获取顶点布局
	VertexLayout GetLayout() const override
	{
		static VertexLayout cachedLayout = VertexType::GetLayout();
		return cachedLayout;
	}

	// 获取顶点步长
	UINT GetStride() const override
	{
		return VertexType::GetStride();
	}

	// 获取类型名称（使用typeid）
	const char* GetTypeName() const override
	{
		return typeid(VertexType).name();
	}

	// 获取类型ID（注册表查找）
	const char* GetTypeId() const override
	{
		static const char* typeId = typeid(VertexType).name();
		return typeId;
	}

	//单例
	static SVertexFactory& Get()
	{
		static SVertexFactory instance;
		return instance;
	}
};



//工厂注册表
//管理顶点工厂的

class SVertexFactoryRegistry
{
public:

	//自动创建工厂
	template<typename VertexType>
	static void Register()
	{
		const char* typeId = typeid(VertexType).name();
		GetFactories()[typeId] = std::make_unique<SVertexFactory<VertexType>>();
	}

	//获取顶点工厂:直接通过类型
	template<typename VertexType>
	static SVertexFactoryBase* Get()
	{
		const char* typeId = typeid(VertexType).name();
		auto it = GetFactories().find(typeId);
		return (it != GetFactories().end()) ? it->second.get() ::nullptr;
	}

	//获取顶点工厂:通过ID字符串
	static SVertexFactoryBase* Get(const char* typeId)
	{
		auto it = GetFactories().find(typeId);
		return (it != GetFactories().end()) ? it->second.get() : nullptr;
	}


	//注册区
	static void RegisterAll()
	{
		// 基础顶点类型
		Register<SPositionColorVertex>();
		Register<SPositionNormalTexVertex>();
		Register<SPositionNormalTexTangentVertex>();
		Register<SPositionNormalTexTangentBinormalVertex>();
		Register<SPositionColorNormalTexVertex>();

		// 骨骼动画顶点，还没有，用到再说
		

		// 新增写下面
	}

	//检查是否注册
	template<typename VertexType>
	static bool IsRegistered()
	{
		const char* typeId - typeid(VertexType).name();
		return GetFactories().find(typeId) != GetFactories().end();
	}

	//获取已经注册过的工厂数量
	static size_t GetRegisteredCount()
	{
		return GetFactories().size();
	}

	static void Clear()
	{
		GetFactories().clear();
	}


	//遍历
	template<typename Func>
	static void ForEach(Func func)
	{
		for (const auto& pair : GetFactories())
		{
			func(pair.second.get());
		}
	}


private:
	static std::unordered_map<std::string,std::unique_ptr<SVertexFactoryBase>>
		& GetFactories()
	{
		static std::unordered_map
			<std::string, std::unique_ptr<SVertexFactoryBase>> 
			factories;

		return factories;
	}
};


//宏定义:定义新的顶点类型并自动注册
#define DEFINE_VERTEX_TYPE(TypeName, ...) \
    using TypeName = SVertex<__VA_ARGS__>; \
    namespace VertexFactoryInit { \
        struct TypeName##_Registrar { \
            TypeName##_Registrar() { \
                SVertexFactoryRegistry::Register<TypeName>(); \
            } \
        }; \
        static TypeName##_Registrar TypeName##_Registrar_Instance; \
    }