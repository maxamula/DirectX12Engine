#pragma once
#include "common.h"
#include "scenegraph.h"

namespace engine
{
	class IScript
	{
	public:
		IScript() = delete;
		explicit IScript(GameObject& obj)
			: m_object(obj)
		{}
		virtual void Begin() = 0;
		virtual void Update(float fElapsedTime) = 0;
	protected:
		GameObject& m_object;
	};

	using ScriptPtr = std::unique_ptr<IScript>;								// Pointer to script instance
	using ScriptCreator = ScriptPtr(*)(GameObject& object);					// Pointer to CreateScript inst

	struct SCIPT_INFO
	{
		uint64_t id;
		ScriptCreator creator;
	};

	struct Script
	{
		ScriptPtr instance{};
	};

	template <class Script_Impl>
	ScriptPtr CreateScript(GameObject& obj)
	{
		return std::make_unique<Script_Impl>(obj);
	}

	ENGINE_API bool LoadGCDLL(const wchar_t* path);
	ENGINE_API bool UnloadGCDLL();
}