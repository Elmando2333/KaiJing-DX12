#pragma once
#pragma comment(lib, "d3dcompiler.lib")
#include <d3d12.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <string>
#include <unordered_map>



struct ShaderInfo
{
    std::string Name;           
    std::wstring SourcePath;    
    std::wstring CompiledPath;
    std::string EntryPoint;     
    std::string Target;         
    bool UseRuntimeCompile;     
};
class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

    void Initialize(const std::wstring& shaderDirectory = L"Shaders/");


    //注册
    void RegisterShader(
        const std::string& shaderName,
        const std::wstring& sourcePath,
        const std::string& entryPoint,
        const std::string& target,
        bool allowRuntimeCompile = true
    );


    //这个注册已编译的
    void RegisterCompiledShader(
        const std::string& shaderName,
        const std::wstring& csoPath
    );

    //加载：先cso没有就再运行编译
    ID3DBlob* LoadShader(const std::string& shaderName);


    //编译
    ID3DBlob* CompileShader(
        const std::wstring& sourcePath,
        const std::string& entryPoint,
        const std::string& target,
        const std::string& shaderName = ""
    );

    //文件加载已经编译好的
    ID3DBlob* LoadCompiledShader(const std::wstring& csoPath);


    //从缓存加载
    ID3DBlob* GetShader(const std::string& shaderName);


    //检查
    bool IsShaderLoaded(const std::string& shaderName) const;

    //预
    bool PreloadAllShaders();

    //清缓存
    void Clear();

    //运行时编译
    void SetRuntimeCompilationEnabled(bool enabled) { m_runtimeCompileEnabled = enabled; }
    bool IsRuntimeCompilationEnabled() const {
        return m_runtimeCompileEnabled;
    }


private:

    //读二进制
    bool ReadFileToBlob(const std::wstring & filePath, ID3DBlob * *ppBlob);

    UINT GetCompileFlags() const;

    std::unordered_map<std::string, ShaderInfo> m_shaderInfos;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> m_shaderCache;


    std::wstring m_shaderDirectory;
    bool m_runtimeCompileEnabled;


#if defined(_DEBUG)
    static constexpr bool DEFAULT_RUNTIME_COMPILE = true;
#else
    static constexpr bool DEFAULT_RUNTIME_COMPILE = false;
#endif
};