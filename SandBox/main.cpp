#include <iostream>
#include "common.h"
#include "engine.h"
#include "window.h"

using namespace engine;

Window* g_mainWindow = nullptr;
Window* g_mainWindow1 = nullptr;

LRESULT CALLBACK MyProc(Window* This, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CLOSE:
		/*if (MessageBox(NULL, L"Are you sure?", L"Exit", MB_OKCANCEL) == IDOK)
			PostQuitMessage(1);*/
		break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	engine::Init();
	// init
	GFX_WND_DESC wndDesc;

	wndDesc.callback = (WndProcFnPtr)MyProc;
	wndDesc.hParent = nullptr;
	wndDesc.szCaption = (wchar_t*)L"My window";
	wndDesc.width = 600;
	wndDesc.height = 400;
	g_mainWindow = Window::Create(NULL, wndDesc);
	g_mainWindow->ShowWnd();
	/*wndDesc.szCaption = (wchar_t*)L"My window 1";
	wndDesc.width = 800;
	wndDesc.height = 600;
	g_mainWindow1 = Window::Create(NULL, wndDesc);
	g_mainWindow1->ShowWnd();*/
	bool isRunning = true;
	MSG msg;
	while (isRunning)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				isRunning = false;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// TODO render
	}
	engine::Shutdown();

	return 0;
}