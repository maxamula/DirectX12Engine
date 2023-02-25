#include "Application.h"


#include <string_view>
#include "Dependencies/nameof.hpp"
#include "Graphics/graphics.h"
#include "scenegraph.h"
#include <fstream>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include "Content/content.h"

namespace engine
{
	extern HINSTANCE g_hInstance;
}

using namespace engine;

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{
	engine::g_hInstance = hInstance;
	engine::gfx::InitD3D();
	
	char p[] = "dssfghjffjkfdjkdsfjvvsdljsokgjpwo";
	char f[] = "fsdfsdsadf";
	char m[] = "341435435431gdffgdssfg";
	char l[] = "sssssssssssss";
	content::RESOURCE_BLOB blob = { 0, std::size(p), (uint8_t*)&p[0]};
	content::RESOURCE_BLOB blob1 = { 0, std::size(f), (uint8_t*)&f[0] };
	content::RESOURCE_BLOB blob2 = { 0, std::size(m), (uint8_t*)&m[0] };
	content::RESOURCE_BLOB blob3 = { 0, std::size(l), (uint8_t*)&l[0] };
	content::RESOURCE_BLOB blobs[] = { blob, blob1, blob2, blob3 };
	content::CreateAssetFile("test.bin", blobs, 4);

	engine::gfx::ShutdownD3D();
	return 0;
}