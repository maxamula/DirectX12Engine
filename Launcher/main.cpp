#include <windows.h>
#include <crtdbg.h>
#include <stdint.h>
#include <unordered_map>
#include <thread>

#include "script.h"

struct SCIPT_INFO
{
	engine::ScriptCreator creator;
	std::string name;
};

typedef void(*ASSEMBLY_MAIN)(uint32_t reason);
ASSEMBLY_MAIN AssemblyMain;
typedef std::unordered_map<uint64_t, SCIPT_INFO>(*SCRIPT_TABLE)();
SCRIPT_TABLE ScriptTable;
HMODULE hGameAssembly;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG
	hGameAssembly = LoadLibrary(L"GameAssembly.dll");
	ScriptTable = (SCRIPT_TABLE)GetProcAddress(hGameAssembly, "ScriptTable");
	AssemblyMain = (ASSEMBLY_MAIN)GetProcAddress(hGameAssembly, "AssemblyMain");
	engine::Init();
	AssemblyMain(0);
	bool isRunning = true;
	std::thread logicThread([&isRunning]() 
	{
		while (isRunning) 
		{
			engine::Update();
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
				logicThread.join();
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		engine::Render();
	}
	AssemblyMain(1);
	FreeLibrary(hGameAssembly);
	engine::Shutdown();
	return 0;
}