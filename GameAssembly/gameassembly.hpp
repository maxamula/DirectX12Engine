#pragma once
#include <unordered_map>
using namespace engine;

// Global decl

__declspec(dllexport) std::unordered_map<uint64_t, SCIPT_INFO> ScriptTable()	// guaranteed to be initialized before main
{
	static std::unordered_map<uint64_t, SCIPT_INFO> table;
	return table;
}

#ifndef _DEBUG
uint8_t RegisterScript(uint64_t handle, ScriptCreator creator)
{
	ScriptTable()[handle] = { creator, ""};
	return 0;
}
#else
uint8_t RegisterScript(uint64_t handle, ScriptCreator creator, const char* name)
{
	ScriptTable()[handle] = { creator, name };
	return 0;
}
#endif