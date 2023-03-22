#include "engine.h"
#include "Graphics/graphics.h"

namespace engine
{
	void Init()
	{
		gfx::InitD3D();
	}
	void Shutdown()
	{
		gfx::ShutdownD3D();
	}
}