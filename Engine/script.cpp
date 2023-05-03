#include "script.h"

namespace engine
{
	typedef std::unordered_map<uint64_t, engine::SCIPT_INFO>& (*SCRIPT_TABLE)();
	HMODULE hGCDll = nullptr;
	SCRIPT_TABLE ScriptTable;

	bool LoadGCDLL(const wchar_t* dllPath)
	{
		if (hGCDll)
			return false;
		hGCDll = LoadLibraryW(dllPath);
		if (hGCDll)
		{
			ScriptTable = (SCRIPT_TABLE)GetProcAddress(hGCDll, "ScriptTable");
			assert(ScriptTable);
			return true;
		}
		else
		{
			return false;
		}
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

	std::unordered_map<uint64_t, engine::SCIPT_INFO>& GetScriptTable()
	{
		return ScriptTable();
	}
}