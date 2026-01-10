#include "TestApp.h"

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	TestApp app(hInstance);
	app.SetWindowTitle("KaiJing Engine");
	app.SetClientWidth(1280);
	app.SetClientHeight(720);
	return app.Run();
}

