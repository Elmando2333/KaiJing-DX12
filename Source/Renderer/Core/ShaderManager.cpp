#include "ShaderManager.h"
#include <iostream>
#include <fstream>

ShaderManager::ShaderManager()
	:m_shaderDirectory(L"Shaders/")
	, m_runtimeCompileEnabled(DEFAULT_RUNTIME_COMPILE)
{

}

ShaderManager::~ShaderManager()
{
	Clear();
}

void ShaderManager::Initialize(const std::wstring& shaderDirectory)
{
	m_shaderDirectory = shaderDirectory;

	if (!m_shaderDirectory.empty())
	{
		wchar_t lastChar = m_shaderDirectory.back();
		if (lastChar != L'/' && lastChar != '\\')
		{
			m_shaderDirectory += L'/';
		}
	}
}

void ShaderManager::RegisterShader
	(const std::string& shaderName, 
	const std::wstring& sourcePath, 
	const std::string& entryPoint, 
	const std::string& target, 
	bool allowRuntimeCompile)
{
	ShaderInfo info;
	info.Name = shaderName;
	info.SourcePath = sourcePath;

	if (!sourcePath.empty())
	{
		bool isAbsolute = (sourcePath.find(L":") != std::wstring::npos) ||
			(sourcePath.length() > 0 && sourcePath[0] == L'\\');
		if (!isAbsolute)
		{
			info.SourcePath = m_shaderDirectory + sourcePath;
		}

	}


	//.cso注意这里只存可能的路径，并不检查存在
	if (!sourcePath.empty())
	{
		std::wstring csoPath = info.SourcePath;
		size_t extPos = csoPath.find_last_of(L".");
		if (extPos != std::wstring::npos)
		{
			csoPath = csoPath.substr(0, extPos) + L".cso";
			info.CompiledPath = csoPath;
		}
	}


	info.EntryPoint = entryPoint;
	info.Target = target;
	info.UseRuntimeCompile = allowRuntimeCompile && m_runtimeCompileEnabled;

	m_shaderInfos[shaderName] = info;
}

void ShaderManager::RegisterCompiledShader(const std::string& shaderName, const std::wstring& csoPath)
{
	ShaderInfo info;
	info.Name = shaderName;
	info.CompiledPath = csoPath;

	if (!csoPath.empty())
	{
		bool isAbsolute = (csoPath.find(L":") != std::wstring::npos) ||
			(csoPath.length() > 0 && csoPath[0] == '\\');

		if (!isAbsolute)
		{
			info.CompiledPath = m_shaderDirectory + csoPath;
		}
	}

	info.UseRuntimeCompile = false;
	m_shaderInfos[shaderName] = info;
}

ID3DBlob* ShaderManager::LoadShader(const std::string& shaderName)
{

	auto cacheIt = m_shaderCache.find(shaderName);
	if (cacheIt != m_shaderCache.end())
	{
		return cacheIt->second.Get();
	}
	


	auto infoIt = m_shaderInfos.find(shaderName);
	if (infoIt == m_shaderInfos.end())
	{
		std::cerr << "Shader not registered: " << shaderName << std::endl;
		return nullptr;
	}

	const ShaderInfo & info= infoIt->second;
	ID3DBlob* byteCode = nullptr;

	if (!info.CompiledPath.empty())
	{
		byteCode = LoadCompiledShader(info.CompiledPath);
		if (byteCode != nullptr)
		{
			Microsoft::WRL::ComPtr<ID3DBlob> cached;
			cached.Attach(byteCode);
			m_shaderCache[shaderName] = cached;
			return cached.Get();
		}
	}


	//这里如果cso不存在，但是可以运行时编译，那就现编
	if (byteCode == nullptr && info.UseRuntimeCompile && !info.SourcePath.empty())
	{
		byteCode = CompileShader(info.SourcePath, info.EntryPoint, info.Target, shaderName);
		if (byteCode != nullptr)
		{
			Microsoft::WRL::ComPtr<ID3DBlob> cached;
			cached.Attach(byteCode);
			m_shaderCache[shaderName] = cached;
			return cached.Get();
		}
	}


	//没一个成功
	std::cerr << "Failed to load shader:" << shaderName << std::endl;
	return nullptr;
}

ID3DBlob* ShaderManager::CompileShader(const std::wstring& sourcePath, const std::string& entryPoint, const std::string& target, const std::string& shaderName)
{
	UINT compileFlags = GetCompileFlags();
	Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errors = nullptr;

	// 开始编译着色器
	HRESULT hr = D3DCompileFromFile(
		sourcePath.c_str(),           // 源文件路径
		nullptr,                      // 宏定义
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  // 包含文件处理
		entryPoint.c_str(),           // 入口函数名
		target.c_str(),               // 着色器目标（vs_5_1, ps_5_1 等）
		compileFlags,                 // 编译标志
		0,                            // 效果标志
		&byteCode,                    // 输出的字节码
		&errors                       // 错误信息
	);

	if (errors!=nullptr)
	{
		const char* errorMsg = (const char*)errors->GetBufferPointer();
		OutputDebugStringA(errorMsg);
		std::cerr << "Shader compilation errors";

		if (!shaderName.empty())
		{
			std::cerr << "for" << shaderName;
		}
		std::cerr << ":" << std::endl;
		std::cerr << errorMsg << std::endl;

	}

	//检查一下结果
	if (FAILED(hr))
	{
		std::wcerr << L"Failed to compile shader:" << sourcePath << std::endl;
		return nullptr;
	}


	return byteCode.Detach();
}

ID3DBlob* ShaderManager::LoadCompiledShader(const std::wstring& csoPath)
{
	std::ifstream fin(csoPath, std::ios::binary);
	if (!fin.is_open())
	{
		return nullptr;
	}

	fin.seekg(0, std::ios::end);
	size_t fileSize = fin.tellg();
	fin.seekg(0, std::ios::beg);

	if (fileSize == 0)
	{
		std::wcerr << L"Shader file is empty:" << csoPath << std::endl;
		return nullptr;
	}

	Microsoft::WRL::ComPtr<ID3DBlob> blob;
	HRESULT hr = D3DCreateBlob(fileSize, &blob);
	if (FAILED(hr))
	{
		std::wcerr << L"Failed to create blob for shader:" << csoPath << std::endl;
		return nullptr;
	}

	fin.read((char*)blob->GetBufferPointer(), fileSize);
	fin.close();

	if (fin.fail())
	{
		std::wcerr << L"Failed to read blob for shader:" << csoPath << std::endl;
		return nullptr;
	}


	return blob.Detach();
}

ID3DBlob* ShaderManager::GetShader(const std::string& shaderName)
{
	auto it = m_shaderCache.find(shaderName);
	if (it!= m_shaderCache.end())
	{
		return it->second.Get();
	}
	return nullptr;
}

bool ShaderManager::IsShaderLoaded(const std::string& shaderName) const
{
	return m_shaderCache.find(shaderName)!=m_shaderCache.end();
}

bool ShaderManager::PreloadAllShaders()
{
	bool allSucess = true;
	for (const auto& pair : m_shaderInfos)
	{
		ID3DBlob* shader = LoadShader(pair.first);
		if (shader == nullptr)
		{
			std::cerr << "Failed to preload shader" << pair.first << std::endl;
			allSucess = false;
		}
	}
	return allSucess;
}

void ShaderManager::Clear()
{
	m_shaderCache.clear();
}


//其实和loadcompiledshader一样
bool ShaderManager::ReadFileToBlob(const std::wstring& filePath, ID3DBlob** ppBlob)
{
	std::ifstream fin(filePath, std::ios::binary);
	if (!fin.is_open())
	{
		return false;
	}

	fin.seekg(0, std::ios::end);
	size_t fileSize = fin.tellg();
	fin.seekg(0, std::ios::beg);

	if (fileSize == 0)
	{
		return false;
	}

	Microsoft::WRL::ComPtr<ID3DBlob> blob;
	HRESULT hr = D3DCreateBlob(fileSize, &blob);
	if (FAILED(hr))
	{
		return false;
	}

	fin.read((char*)blob->GetBufferPointer(), fileSize);
	fin.close();

	if (fin.fail())
	{
		return false;
	}

	*ppBlob = blob.Detach();
	return true;

}

UINT ShaderManager::GetCompileFlags() const
{
	UINT flags = 0;
#if defined(_DEBUG)
	flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	return flags;
}
