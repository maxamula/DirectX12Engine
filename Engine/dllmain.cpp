#include <windows.h>
#include <assert.h>

namespace engine
{
    HINSTANCE g_hInstance = nullptr;
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        engine::g_hInstance = hinstDLL;
        break;
    case DLL_PROCESS_DETACH:
        
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