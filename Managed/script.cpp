#include "Script.h"
#include <msclr/marshal_cppstd.h>

namespace Engine
{
	public ref class ScriptInfo
	{
	public:
		property System::String^ Name;
		property uint64_t Id;
	};

	public ref class Scripting
	{
	public:
		static bool LoadGCDLL(System::String^ path)
		{
			return engine::LoadGCDLL(msclr::interop::marshal_as<std::wstring>(path).c_str());
		}

		static void UnloadGCDLL()
		{
			engine::UnloadGCDLL();
		}

		static array<ScriptInfo^>^ GetScriptNames()
		{
			std::unordered_map<uint64_t, engine::SCIPT_INFO>& scriptTable = engine::GetScriptTable();
			array<ScriptInfo^>^ names = gcnew array<ScriptInfo^>(scriptTable.size());
			uint32_t i = 0;
			for (auto it = scriptTable.begin(); it != scriptTable.end(); it++, i++)
			{
				System::String^ name = gcnew System::String(it->second.name.c_str());
				names[i] = gcnew ScriptInfo();
				names[i]->Name = name;
				names[i]->Id = it->first;
			}
			return names;
		}
	};
}