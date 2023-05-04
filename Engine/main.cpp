#include <iostream>
#include "common.h"
#include "engine.h"
#include "window.h"
#include "Graphics/buffers.h"
#include "Graphics/content.h"
#pragma warning(disable:4996)

using namespace engine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	AllocConsole();
	freopen("CONOUT$", "w", stderr);
#endif
	engine::Init();

	engine::Shutdown();
	return 0;
}