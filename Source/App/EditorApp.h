#pragma once
#include "Core/KJApp.h"
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx12.h"
#include <vector>
#include <string>

//这个编辑器左边是场景视口，右边是检查器那种可以拖动的

class EditorApp :public KJApp
{
public:
	EditorApp(HINSTANCE hInstance);
	virtual ~EditorApp();



protected:
	bool Initialize() override;
	void Update(float deltaTime) override;
	void Draw() override;
	void OnResize() override;


private:

	//imgui相关的
	bool InitializeImGui();
	void ShutdownImGui();
	//void RenderImGui();

	//UI的
	void DrawMainMenuBar();//头顶
	void DrawSceneView();//左视图
	void DrawInspectorPanel();//右面板


	//先来个场景
	struct SceneObject
	{
		std::string name;
		float position[3] = { 0,0,0 };
		float rotation[3] = { 0,0,0 };
		float scale[3] = { 1,1,1 };
	};

	std::vector<SceneObject> m_sceneObjects;
	int m_selectedObjectIndex = -1;


	//Imgui
	ID3D12DescriptorHeap* m_imguiSrvHeap = nullptr;
	bool m_showDemoWindow = false;
	bool m_dockspaceInitialized = false;  // 标记 DockSpace 是否已初始化默认布局

};