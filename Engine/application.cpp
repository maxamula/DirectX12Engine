#include "Application.h"


#include <string_view>
#include "Dependencies/nameof.hpp"
#include "Graphics/graphics.h"
#include "scenegraph.h"
#include <fstream>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include "Content/content.h"
#include "Graphics/rendersurface.h"


using namespace engine;

namespace engine
{
	HINSTANCE g_hInstance;
	extern LRESULT CALLBACK WndProcBase(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
}

LRESULT CALLBACK MainWndProc(Window* wnd, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = engine::WndProcBase;
	wc.cbClsExtra = 0;
	wc.lpszClassName = WND_CLASS;
	wc.cbWndExtra = sizeof(void*);	// 8 bytes
	wc.hInstance = GetModuleHandle(NULL);
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wc);
	engine::gfx::InitD3D();

	HWND a = CreateWindow(WND_CLASS, L"s", WS_OVERLAPPED, 0, 0, 100, 200, nullptr, nullptr, nullptr, NULL);

	gfx::RenderSurface surf = gfx::RenderSurface(a,100, 200);
	//MSG msg;
	//while (GetMessage(&msg, NULL, 0, 0))
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}
	surf.Release();
	engine::gfx::ShutdownD3D();
	UnregisterClass(WND_CLASS, wc.hInstance);
	return 0;
}