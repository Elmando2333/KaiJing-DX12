// VertexFactory.h
#pragma once
#include "Renderer/Resources/Vertex.h"
#include "Renderer/Resources/VertexLayout.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <typeinfo>
#include <cstdint>

/**
 * @brief 顶点工厂基类
 * @details 提供统一的接口访问顶点类型信息（运行时查找）
 * @note 如果编译时已知顶点类型，直接使用模板类型即可，无需工厂
 */
class SVertexFactoryBase
{
public:
    virtual ~SVertexFactoryBase() = default;

    /**
     * @brief 获取顶点布局
     */
    virtual VertexLayout GetLayout() const = 0;

    /**
     * @brief 获取顶点步长
     */
    virtual uint32_t GetStride() const = 0;

    /**
     * @brief 获取顶点类型名称（typeid名称）
     */
    virtual const char* GetTypeName() const = 0;

    /**
     * @brief 获取类型ID（用于注册表查找）
     */
    virtual const char* GetTypeId() const = 0;
};


/**
 * @brief 模板顶点工厂
 * @details 为任何顶点类型创建工厂，减少代码重复
 */
template<typename VertexType>
class SVertexFactory : public SVertexFactoryBase
{
public:
    /**
     * @brief 获取顶点布局
     */
    VertexLayout GetLayout() const override
    {
        static VertexLayout cachedLayout = VertexType::GetLayout();
        return cachedLayout;
    }

    /**
     * @brief 获取顶点步长
     */
    uint32_t GetStride() const override
    {
        return VertexType::GetStride();
    }

    /**
     * @brief 获取类型名称（使用typeid）
     */
    const char* GetTypeName() const override
    {
        return typeid(VertexType).name();
    }

    /**
     * @brief 获取类型ID（用于注册表查找）
     */
    const char* GetTypeId() const override
    {
        static const char* typeId = typeid(VertexType).name();
        return typeId;
    }

    /**
     * @brief 获取单例实例
     */
    static SVertexFactory& Get()
    {
        static SVertexFactory instance;
        return instance;
    }
};


/**
 * @brief 顶点工厂注册表
 * @details 管理所有注册的顶点类型，支持运行时查找
 * @note 主要用于序列化、配置文件加载等需要运行时查找的场景
 */
class SVertexFactoryRegistry
{
public:
    /**
     * @brief 注册顶点类型
     */
    template<typename VertexType>
    static void Register()
    {
        const char* typeId = typeid(VertexType).name();
        GetFactories()[typeId] = std::make_unique<SVertexFactory<VertexType>>();
    }

    /**
     * @brief 获取顶点工厂（通过类型）
     */
    template<typename VertexType>
    static SVertexFactoryBase* Get()
    {
        const char* typeId = typeid(VertexType).name();
        auto it = GetFactories().find(typeId);
        return (it != GetFactories().end()) ? it->second.get() : nullptr;
    }

    /**
     * @brief 获取顶点工厂（通过类型ID字符串）
     */
    static SVertexFactoryBase* Get(const char* typeId)
    {
        if (!typeId)
        {
            return nullptr;
        }
        auto it = GetFactories().find(typeId);
        return (it != GetFactories().end()) ? it->second.get() : nullptr;
    }

    /**
     * @brief 通过类型ID字符串获取布局
     */
    static VertexLayout GetLayout(const char* typeId)
    {
        auto* factory = Get(typeId);
        if (factory)
        {
            return factory->GetLayout();
        }
        throw std::runtime_error("Vertex type not found: " + std::string(typeId ? typeId : "null"));
    }

    /**
     * @brief 注册所有常用顶点类型
     */
    static void RegisterAll()
    {
        // 基础顶点类型
        Register<SPositionColorVertex>();
        Register<SPositionNormalTexVertex>();
        Register<SPositionNormalTexTangentVertex>();
        Register<SPositionNormalTexTangentBinormalVertex>();
        Register<SPositionColorNormalTexVertex>();

        // TODO: 骨骼动画顶点，用到时再添加
    }

    /**
     * @brief 检查顶点类型是否已注册
     */
    template<typename VertexType>
    static bool IsRegistered()
    {
        const char* typeId = typeid(VertexType).name();
        return GetFactories().find(typeId) != GetFactories().end();
    }

    /**
     * @brief 获取已注册的工厂数量
     */
    static size_t GetRegisteredCount()
    {
        return GetFactories().size();
    }

    /**
     * @brief 清空所有注册
     */
    static void Clear()
    {
        GetFactories().clear();
    }

    /**
     * @brief 遍历所有注册的工厂
     */
    template<typename Func>
    static void ForEach(Func func)
    {
        for (const auto& pair : GetFactories())
        {
            func(pair.second.get());
        }
    }

private:
    /**
     * @brief 获取工厂映射表
     */
    static std::unordered_map<std::string, std::unique_ptr<SVertexFactoryBase>>& GetFactories()
    {
        static std::unordered_map<std::string, std::unique_ptr<SVertexFactoryBase>> factories;
        return factories;
    }
};


/**
 * @brief 宏定义：定义新的顶点类型并自动注册到工厂
 * @details 使用此宏定义的顶点类型会自动注册到工厂注册表
 * @example DEFINE_VERTEX_TYPE(MyVertex, VertexComponents::Position, VertexComponents::Color)
 */
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