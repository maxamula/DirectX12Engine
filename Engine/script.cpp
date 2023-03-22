#include "script.h"


namespace engine
{
	HMODULE hGCDll = nullptr;
	typedef std::unordered_map<uint64_t, SCIPT_INFO>&(*SCRIPT_TABLE)();
	SCRIPT_TABLE ScriptTable;

	bool LoadGCDLL(const wchar_t* dllPath)	// Only used with editor
	{
		if (hGCDll)
			return false;
		hGCDll = LoadLibraryW(dllPath);
		ScriptTable = (SCRIPT_TABLE)GetProcAddress(hGCDll, "ScriptTable");
		assert(hGCDll);
		return hGCDll ? true : false;
	}

	bool UnloadGCDLL()
	{
		if (!hGCDll)
			return false;
		bool succseeded = FreeLibrary(hGCDll);
		if (succseeded)
			hGCDll = nullptr;
		return succseeded;
	}
}