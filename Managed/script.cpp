#include "Script.h"
#include <msclr/marshal_cppstd.h>

namespace Engine
{
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
	};
}