#include <windows.h>
#include "engine.h"

using namespace engine;

//APPLICATION_INFO g_appInfo{};
//std::vector<Window*> g_windows;
//
//INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
//{
//#ifdef _DEBUG
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	AllocConsole();
//	freopen("CONOUT$", "w", stderr);
//#endif
//	engine::Init();
//	g_appInfo = LoadApplicationInfo(L"game.bin");
//	if (g_appInfo.windows.size())
//	{
//		g_windows.reserve(g_appInfo.windows.size());
//		for (uint32_t i = 0; i < g_appInfo.windows.size(); i++)
//		{
//			g_windows.emplace_back(reinterpret_cast<Window*>(Window::Create(hInstance, g_appInfo.windows[i])));
//		}
//	}
//}