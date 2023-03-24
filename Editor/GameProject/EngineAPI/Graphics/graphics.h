#pragma once
//directx 12
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
// link libraries
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// Graphics modules
#include "cmdqueue.h"
#include "descriptorheap.h"
#include "shaders.h"
#include "gpass.h"


namespace engine::gfx
{
	class RenderSurface;
	class Texture;
	class CommandQueue;
	class DescriptorHeap;

	extern ID3D12Device* device;
	extern IDXGIFactory7* dxgiFactory;
	extern IDXGIAdapter4* dxgiAdapter;
	extern CommandQueue g_cmdQueue;
	extern DescriptorHeap g_rtvHeap;
	extern DescriptorHeap g_dsvHeap;
	extern DescriptorHeap g_srvHeap;
	extern DescriptorHeap g_uavHeap;
	
	void InitD3D();
	void ShutdownD3D();
}