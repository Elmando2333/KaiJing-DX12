#include "Core/KJUtil.h"
#include <fstream>
#include <sstream>

namespace KJUtil
{
	void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw std::runtime_error("DX12 Error");
		}
	}

	void ThrowIfFailed(HRESULT hr, const char* message)
	{
		if (FAILED(hr))
		{
			std::string errorMsg = message;
			errorMsg += " HRESULT: 0x";
			std::ostringstream oss;
			oss << std::hex << hr;
			errorMsg += oss.str();
			throw std::runtime_error(errorMsg);
		}
	}

	Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target
	)
	{
		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errors = nullptr;

		LPCWSTR wideFilename = filename.c_str();

		HRESULT hr = D3DCompileFromFile(
			wideFilename,
			defines,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entrypoint.c_str(),
			target.c_str(),
			compileFlags,
			0,
			byteCode.GetAddressOf(),
			errors.GetAddressOf()
		);

		if (errors != nullptr)
		{
			const char* errorMsg = static_cast<const char*>(errors->GetBufferPointer());
			OutputDebugStringA(errorMsg);
		}

		ThrowIfFailed(hr, "Shader compilation failed");

		return byteCode;
	}

	Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const std::wstring& filename)
	{
		std::vector<BYTE> data = ReadBinaryFile(filename);

		Microsoft::WRL::ComPtr<ID3DBlob> blob;
		ThrowIfFailed(D3DCreateBlob(data.size(), &blob));

		memcpy(blob->GetBufferPointer(), data.data(), data.size());

		return blob;
	}

	std::vector<BYTE> ReadBinaryFile(const std::wstring& filename)
	{
		HANDLE hFile = CreateFileW(
			filename.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			throw std::runtime_error("Failed to open file");
		}

		LARGE_INTEGER fileSize;
		if (!GetFileSizeEx(hFile, &fileSize))
		{
			CloseHandle(hFile);
			throw std::runtime_error("Failed to get file size");
		}

		std::vector<BYTE> buffer(static_cast<size_t>(fileSize.QuadPart));
		DWORD bytesRead = 0;
		if (!ReadFile(hFile, buffer.data(), static_cast<DWORD>(fileSize.QuadPart), &bytesRead, nullptr))
		{
			CloseHandle(hFile);
			throw std::runtime_error("Failed to read file");
		}

		CloseHandle(hFile);
		return buffer;
	}

	std::string ReadTextFile(const std::wstring& filename)
	{
		std::vector<BYTE> data = ReadBinaryFile(filename);
		return std::string(reinterpret_cast<const char*>(data.data()), data.size());
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateUploadBuffer(
		ID3D12Device* device,
		UINT64 byteSize)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;

		D3D12_HEAP_PROPERTIES heapProps = {};
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = byteSize;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ThrowIfFailed(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer)
		));

		return uploadBuffer;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer
	)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;

		D3D12_HEAP_PROPERTIES defaultHeapProps = {};
		defaultHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		defaultHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		defaultHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		defaultHeapProps.CreationNodeMask = 1;
		defaultHeapProps.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = byteSize;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ThrowIfFailed(device->CreateCommittedResource(
			&defaultHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&defaultBuffer)
		));

		if (initData != nullptr)
		{
			uploadBuffer = CreateUploadBuffer(device, byteSize);

			void* mappedData = nullptr;
			D3D12_RANGE readRange = { 0, 0 };
			ThrowIfFailed(uploadBuffer->Map(0, &readRange, &mappedData));
			memcpy(mappedData, initData, static_cast<size_t>(byteSize));
			uploadBuffer->Unmap(0, nullptr);

			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = defaultBuffer.Get();
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

			cmdList->ResourceBarrier(1, &barrier);

			cmdList->CopyResource(defaultBuffer.Get(), uploadBuffer.Get());

			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

			cmdList->ResourceBarrier(1, &barrier);
		}

		return defaultBuffer;
	}

	UINT CalculateConstantBufferByteSize(UINT byteSize)
	{
		return (byteSize + 255) & ~255;
	}
}
