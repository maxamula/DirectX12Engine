#include <windows.h>
#include <assert.h>

namespace engine
{
    HINSTANCE g_hInstance = {};
    extern LRESULT CALLBACK WndProcBase(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        engine::g_hInstance = hinstDLL;
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
        Beep(500, 100);
        break;
    case DLL_PROCESS_DETACH:
        UnregisterClass(WND_CLASS, engine::g_hInstance);
        break;
    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        break;
    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        break;
    }
    return TRUE;
}