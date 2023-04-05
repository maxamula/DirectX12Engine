#include "engine.h"
#include "Graphics/graphics.h"
#include "logger.h"
#include <iostream>

#include <Windows.h>
#include <fcntl.h>
#include <io.h>
#include <cstdio>
#include <iostream>
namespace engine
{
	void Init()
	{
		LogInit(GetStdHandle(STD_ERROR_HANDLE));
		gfx::InitD3D();
	}
	void Shutdown()
	{
		gfx::ShutdownD3D();
		LogShutdown();
	}
}