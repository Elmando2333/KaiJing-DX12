#pragma once
#include "KJApp.h"

class TestApp : public KJApp
{
public:
	TestApp(HINSTANCE hInstance) : KJApp(hInstance) {}

protected:
	bool Initialize() override;
	void Update(float deltaTime) override;
	void Draw() override;
	void OnResize() override;
};

