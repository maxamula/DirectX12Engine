#include "Script.h"
#include <msclr/marshal_cppstd.h>

namespace Engine
{
	public ref class Scripting
	{
	public:
		static void LoadGCDLL(System::String^ path)
		{
			engine::LoadGCDLL(msclr::interop::marshal_as<std::wstring>(path).c_str());
		}

		static void UnloadGCDLL()
		{
			engine::UnloadGCDLL();
		}
	};
}