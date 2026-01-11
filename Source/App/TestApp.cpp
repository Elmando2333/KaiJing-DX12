#include "App/TestApp.h"

bool TestApp::Initialize()
{
	return true;
}

void TestApp::Update(float deltaTime)
{
}

void TestApp::Draw()
{
	auto& device = GetDevice();
	auto& swapChain = GetSwapChain();

	device.ResetCommandList();

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChain.GetCurrentRTVHandle(GetRTVHeap());
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetDepthStencilBuffer().GetDSVHandle(GetDSVHeap(), 0);

	device.GetCommandList()->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

	const float clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	device.GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	device.GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	auto& viewport = GetViewport();
	device.GetCommandList()->RSSetViewports(1, &viewport.GetViewport());
	device.GetCommandList()->RSSetScissorRects(1, &viewport.GetScissorRect());

	device.GetCommandList()->Close();

	ID3D12CommandList* cmdLists[] = { device.GetCommandList() };
	device.GetCommandQueue()->ExecuteCommandLists(1, cmdLists);

	swapChain.Present(1, 0);
	device.GetMainFence().Flush(device.GetCommandQueue());
}

void TestApp::OnResize()
{
}

