#pragma once
#include <unordered_map>
using namespace engine;

// Global decl
std::set<int> existing_ids;

__declspec(dllexport) std::unordered_map<uint64_t ,SCIPT_INFO> ScriptTable()	// guaranteed to be initialized before main
{
	static std::unordered_map<uint64_t, SCIPT_INFO> table;
	return table;
}

uint8_t RegisterScript(uint64_t handle, ScriptCreator creator)
{
	ScriptTable()[handle] = { handle, creator };
	return 0;
}