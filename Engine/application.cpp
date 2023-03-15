#include "Application.h"


#include <string_view>
#include "Dependencies/nameof.hpp"
#include "Graphics/graphics.h"
#include "scenegraph.h"
#include <fstream>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include "Content/content.h"
#include "Graphics/rendersurface.h"


using namespace engine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	engine::gfx::InitD3D();

	engine::gfx::ShutdownD3D();
	return 0;
}