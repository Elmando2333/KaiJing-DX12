#include "App/EditorApp.h"
#include "DX12/DX12Device.h"
#include <commdlg.h>
#include "imgui_internal.h"  // 需要 DockBuilder API
#include <iostream>

EditorApp::EditorApp(HINSTANCE hInstance): KJApp(hInstance)
{
	SetWindowTitle("KJ Editor");
	SetClientWidth(1920);
	SetClientHeight(1080);
}

EditorApp::~EditorApp()
{
	//把imgui关了
	ShutdownImGui();
}

bool EditorApp::Initialize()
{
	if (!InitializeImGui())
	{
		return false;
	}



	//场景初始化，先放一个cube，后面再写objloader
	SceneObject obj;
	obj.name = "Cube";
	obj.position[0] = 0.0f;
	obj.position[1] = 0.0f;
	obj.position[2] = 0.0f;
	m_sceneObjects.push_back(obj);




	return true;
}

void EditorApp::Update(float deltaTime)
{
	//
}

void EditorApp::Draw()
{
	auto& device = GetDevice();
	auto& swapChain = GetSwapChain();

	device.ResetCommandList();

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChain.GetCurrentRTVHandle(GetRTVHeap());
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetDepthStencilBuffer().GetDSVHandle(GetDSVHeap(), 0);


	device.GetCommandList()->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);



	//清除一下背景
	const float clearColor[] = { 0.1f,0.1f,0.1f,1.0f };
	device.GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	device.GetCommandList()->ClearDepthStencilView
	(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);


	//视口
	auto& viewport = GetViewport();
	device.GetCommandList()->RSSetViewports(1, &viewport.GetViewport());
	device.GetCommandList()->RSSetScissorRects(1, &viewport.GetScissorRect());

	//Imgui的
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 创建DockSpace
	ImGuiViewport* viewport_imgui = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport_imgui->WorkPos);
	ImGui::SetNextWindowSize(viewport_imgui->WorkSize);
	ImGui::SetNextWindowViewport(viewport_imgui->ID);

	ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;


	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("DockSpace", nullptr, window_flags);
	ImGui::PopStyleVar(3);

	//DockSpace
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	
	if (!m_dockspaceInitialized)
	{
		m_dockspaceInitialized = true;
		
		
		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport_imgui->WorkSize);
		
		
		ImGuiID dock_id_left;
		ImGuiID dock_id_right;
		ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.7f, &dock_id_left, &dock_id_right);
		
		
		ImGui::DockBuilderDockWindow("Scene View", dock_id_left);
		
		
		ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
		
		
		ImGui::DockBuilderFinish(dockspace_id);
	}

	DrawMainMenuBar();

	DrawSceneView();
	DrawInspectorPanel();

	//demo的窗口，不用一般
	if (m_showDemoWindow)
	{
		ImGui::ShowDemoWindow(&m_showDemoWindow);
	}

	ImGui::End();  //这是DockSpace的

	ImGui::Render();


	// 渲染ImGui
	ImDrawData* drawData = ImGui::GetDrawData();
	if (drawData)
	{
		ID3D12DescriptorHeap* heaps[] = { m_imguiSrvHeap };
		device.GetCommandList()->SetDescriptorHeaps(1, heaps);
		ImGui_ImplDX12_RenderDrawData(drawData, device.GetCommandList());
	}
	//

	device.GetCommandList()->Close();

	ID3D12CommandList* cmdLists[] = { device.GetCommandList() };
	device.GetCommandQueue()->ExecuteCommandLists(1, cmdLists);

	swapChain.Present(1, 0);
	device.GetMainFence().Flush(device.GetCommandQueue());
}

bool EditorApp::InitializeImGui()
{
	IMGUI_CHECKVERSION();
	
	
	ImGuiContext* oldCtx = ImGui::GetCurrentContext();
	if (oldCtx != nullptr)
	{
		OutputDebugStringA("存在旧的上下文！\n");
		ImGui::DestroyContext(oldCtx);
	}
	ImGui::SetCurrentContext(nullptr);

	
	ImGuiContext* ctx = ImGui::CreateContext();
	if (ctx == nullptr)
	{
		return false;
	}

	
	ImGui::SetCurrentContext(ctx);

	
	if (ImGui::GetCurrentContext() == nullptr || ImGui::GetCurrentContext() != ctx)
	{
		return false;
	}

	
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // 启用 DockSpace
	
	
	io.DisplaySize = ImVec2(static_cast<float>(GetClientWidth()), static_cast<float>(GetClientHeight()));
    
	
	ImGui::StyleColorsDark();

	
	if (ImGui::GetCurrentContext() == nullptr)
	{
		return false;
	}

	
	if (!ImGui_ImplWin32_Init(GetMainWindow()))
	{
		return false;
	}



	auto& device = GetDevice();
	DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//SRV
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	HRESULT hr = device.GetDevice()->CreateDescriptorHeap(
		&desc,
		IID_PPV_ARGS(&m_imguiSrvHeap)
	);
	if (FAILED(hr))
	{
		return false;
	}


	ImGui_ImplDX12_InitInfo initInfo{};
	initInfo.Device = device.GetDevice();
	initInfo.CommandQueue = device.GetCommandQueue();
	initInfo.NumFramesInFlight = 3;  
	initInfo.RTVFormat = rtvFormat;
	initInfo.DSVFormat = dsvFormat;
	initInfo.SrvDescriptorHeap = m_imguiSrvHeap;


	
	initInfo.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
		{
			*out_cpu_handle = info->SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			*out_gpu_handle = info->SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		};

	initInfo.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE)
		{
			
		};

	if (!ImGui_ImplDX12_Init(&initInfo))
	{
		return false;
	}

	SetImGuiInitialized(true);
	return true;
}

void EditorApp::ShutdownImGui()
{
	if (m_imguiSrvHeap)
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		m_imguiSrvHeap->Release();
		m_imguiSrvHeap = nullptr;
	}
}

void EditorApp::DrawMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		bool fileMenuOpen = ImGui::BeginMenu("File");
		if (fileMenuOpen)
		{
			if (ImGui::MenuItem("Import OBJ...", "Ctrl+O"))
			{
				OPENFILENAMEA ofn;
				char szFile[260] = { 0 };
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = GetMainWindow();
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = "OBJ Files\0*.obj\0All Files\0*.*\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				if (GetOpenFileNameA(&ofn))
				{
					// TODO: Load OBJ file
					SceneObject obj;
					obj.name = szFile;
					m_sceneObjects.push_back(obj);
				}
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "Alt+F4"))
			{
				PostQuitMessage(0);
			}
		}
		if (fileMenuOpen)
		{
			ImGui::EndMenu();
		}

		bool windowMenuOpen = ImGui::BeginMenu("Window");
		if (windowMenuOpen)
		{
			ImGui::MenuItem("ImGui Demo", nullptr, &m_showDemoWindow);
		}
		if (windowMenuOpen)
		{
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void EditorApp::DrawSceneView()
{
	ImGui::Begin("Scene View");

	ImVec2 contentSize = ImGui::GetContentRegionAvail();

	// Render 3D scene here

	ImVec2 canvasPos = ImGui::GetCursorScreenPos();
	ImVec2 canvasSize = ImGui::GetContentRegionAvail();

	if (canvasSize.x < 50.0f) canvasSize.x = 50.0f;
	if (canvasSize.y < 50.0f) canvasSize.y = 50.0f;

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList->AddRectFilled(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
		IM_COL32(30, 30, 30, 255));
	drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
		IM_COL32(100, 100, 100, 255));

	ImVec2 textSize = ImGui::CalcTextSize("3D Scene View");
	ImGui::SetCursorPosX((canvasSize.x - textSize.x) * 0.5f);
	ImGui::SetCursorPosY(canvasSize.y * 0.5f - textSize.y);
	ImGui::Text("3D Scene View");

	textSize = ImGui::CalcTextSize("(Scene will be displayed after rendering pipeline is complete)");
	ImGui::SetCursorPosX((canvasSize.x - textSize.x) * 0.5f);
	ImGui::Text("(Scene will be displayed after rendering pipeline is complete)");

	ImGui::End();
}

void EditorApp::DrawInspectorPanel()
{
	ImGui::Begin("Inspector");

	ImGui::Text("Scene Objects:");
	ImGui::Separator();

	if (m_sceneObjects.empty())
	{
		ImGui::Text("Scene is empty");
	}
	else
	{
		for (size_t i = 0; i < m_sceneObjects.size(); i++)
		{
			bool isSelected = (m_selectedObjectIndex == static_cast<int>(i));
			if (ImGui::Selectable(m_sceneObjects[i].name.c_str(), isSelected))
			{
				m_selectedObjectIndex = static_cast<int>(i);
			}
		}
	}

	ImGui::Separator();

	if (m_selectedObjectIndex >= 0 && m_selectedObjectIndex < static_cast<int>(m_sceneObjects.size()))
	{
		SceneObject& obj = m_sceneObjects[m_selectedObjectIndex];

		ImGui::Text("Properties:");
		ImGui::Text("Name: %s", obj.name.c_str());

		ImGui::Separator();
		ImGui::Text("Transform:");

		ImGui::DragFloat3("Position", obj.position, 0.1f);
		ImGui::DragFloat3("Rotation", obj.rotation, 1.0f);
		ImGui::DragFloat3("Scale", obj.scale, 0.1f);

		ImGui::Separator();

		if (ImGui::Button("Delete Object"))
		{
			m_sceneObjects.erase(m_sceneObjects.begin() + m_selectedObjectIndex);
			m_selectedObjectIndex = -1;
		}
	}
	else
	{
		ImGui::Text("No object selected");
	}

	ImGui::End();
}


void EditorApp::OnResize()
{
	
	if (m_imguiInitialized && ImGui::GetCurrentContext() != nullptr)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(static_cast<float>(GetClientWidth()), static_cast<float>(GetClientHeight()));
	}
}



