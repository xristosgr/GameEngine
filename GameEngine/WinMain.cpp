#include"Engine.h"
#include<Windows.h>

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to call CoInitialize.");
		return -1;
	}

	const int w = 1280;
	const int h = 720;
	Engine engine;
	if (engine.Initialize(hInstance, "DXEngine", "MyWindowClass", w, h))
	{
		while (engine.ProcessMessages() == true)
		{
			engine.Update(w, h);
		}
	}
	return 0;
}