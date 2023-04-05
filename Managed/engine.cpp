#include "engine.h"

namespace Engine
{
	public ref class Core
	{
	public:
		static void Init()
		{
			engine::Init();
		}

		static void Shutdown()
		{
			engine::Shutdown();
		}
	};
}