#include <iostream>
#include "common.h"
#include "engine.h"
#include "window.h"
#pragma warning(disable:4996)

using namespace engine;

Window* g_mainWindow = nullptr;

LRESULT CALLBACK MyProc(Window* This, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CLOSE:
		if (MessageBox(NULL, L"Are you sure?", L"Exit", MB_OKCANCEL) == IDOK)
			PostQuitMessage(1);
		return 0;
		break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	AllocConsole();
	freopen("CONOUT$", "w", stderr);
#endif
	engine::Init();
	GFX_WND_DESC wndDesc;

	wndDesc.callback = (WndProcFnPtr)MyProc;
	wndDesc.hParent = nullptr;
	wndDesc.szCaption = (wchar_t*)L"My window";
	wndDesc.width = 600;
	wndDesc.height = 400;
	g_mainWindow = Window::Create(NULL, wndDesc);
	//g_mainWindow->Fullscreen(true);
	g_mainWindow->ShowWnd();
	bool isRunning = true;

	std::thread thread([&]()
	{
			while (isRunning)
			{
				g_mainWindow->Render();
			}
	});
	MSG msg;
	while (isRunning)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				isRunning = false;
				thread.join();
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// TODO render
		
	}
	g_mainWindow->Destroy();
	engine::Shutdown();

	return 0;
}