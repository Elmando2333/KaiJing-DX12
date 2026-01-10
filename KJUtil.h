#pragma once
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <string>
#include <vector>
#include <stdexcept>

#pragma comment(lib, "d3dcompiler.lib")

namespace KJUtil
{
	void ThrowIfFailed(HRESULT hr);
	void ThrowIfFailed(HRESULT hr, const char* message);

	Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target
	);

	Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const std::wstring& filename);

	std::vector<BYTE> ReadBinaryFile(const std::wstring& filename);
	std::string ReadTextFile(const std::wstring& filename);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateUploadBuffer(
		ID3D12Device* device,
		UINT64 byteSize
	);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer
	);

	UINT CalculateConstantBufferByteSize(UINT byteSize);
}
