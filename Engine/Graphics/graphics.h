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
#include "overlay.h"

#ifndef _DEBUG
#ifdef _DRAW_DEBUG_INFO
#error "Cannot draw debug info in release mode"
#endif
#endif

namespace engine::gfx
{
	class RenderSurface;
	class Texture;
	class CommandQueue;
	class DescriptorHeap;
	class ResourceBarrier;
	class Texture;
	class RenderTexture;
	class DepthTexture;

	struct RENDER_TARGET
	{
		ID3D12Resource* resource = nullptr;
		DESCRIPTOR_HANDLE allocation{};
	};

	struct GFX_FRAME_DESC
	{
		uint32_t rtvCount = 1;
		RENDER_TARGET* renderTargets = nullptr;
		D3D12_VIEWPORT* viewports =	nullptr;
		D3D12_RECT* scissiors = nullptr;
		void(*cbOverlay)(void) = nullptr;
		uint32_t surfWidth;
		uint32_t surfHeight;
	};

	const struct
	{
		const D3D12_RASTERIZER_DESC NO_CULL
		{
			D3D12_FILL_MODE_SOLID,						// D3D12_FILL_MODE FillMode;
			D3D12_CULL_MODE_NONE,						// D3D12_CULL_MODE CullMode;
			0,											// BOOL FrontCounter Clockwise;
			0,											// INT DepthBias;
			0,											// FLOAT DepthBiasClamp;
			0,											// FLOAT Slope Scaled DepthBias;
			1,											// BOOL DepthClipEnable;
			1,											// BOOL MultisampleEnable;
			0,											// BOOL Antialiased LineEnable;
			0,											// UINT Forced SampleCount;
			D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF,	// D3D12_CONSERVATIVE_RASTERIZATION_MODE ConservativeRaster;
		};

		const D3D12_RASTERIZER_DESC BACKFACE_CULL
		{
			D3D12_FILL_MODE_SOLID,						// D3D12_FILL_MODE FillMode;
			D3D12_CULL_MODE_BACK,						// D3D12_CULL_MODE CullMode;
			0,											// BOOL FrontCounter Clockwise;
			0,											// INT DepthBias;
			0,											// FLOAT DepthBiasClamp;
			0,											// FLOAT Slope Scaled DepthBias;
			1,											// BOOL DepthClipEnable;
			1,											// BOOL MultisampleEnable;
			0,											// BOOL Antialiased LineEnable;
			0,											// UINT Forced SampleCount;
			D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF,	// D3D12_CONSERVATIVE_RASTERIZATION_MODE ConservativeRaster;
		};

		const D3D12_RASTERIZER_DESC FRONTFACE_CULL
		{
			D3D12_FILL_MODE_SOLID,						// D3D12_FILL_MODE FillMode;
			D3D12_CULL_MODE_FRONT,						// D3D12_CULL_MODE CullMode;
			0,											// BOOL FrontCounter Clockwise;
			0,											// INT DepthBias;
			0,											// FLOAT DepthBiasClamp;
			0,											// FLOAT Slope Scaled DepthBias;
			1,											// BOOL DepthClipEnable;
			1,											// BOOL MultisampleEnable;
			0,											// BOOL Antialiased LineEnable;
			0,											// UINT Forced SampleCount;
			D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF,	// D3D12_CONSERVATIVE_RASTERIZATION_MODE ConservativeRaster;
		};

		const D3D12_RASTERIZER_DESC WIREFRAME
		{
			D3D12_FILL_MODE_WIREFRAME,						// D3D12_FILL_MODE FillMode;
			D3D12_CULL_MODE_NONE,						// D3D12_CULL_MODE CullMode;
			0,											// BOOL FrontCounter Clockwise;
			0,											// INT DepthBias;
			0,											// FLOAT DepthBiasClamp;
			0,											// FLOAT Slope Scaled DepthBias;
			1,											// BOOL DepthClipEnable;
			1,											// BOOL MultisampleEnable;
			0,											// BOOL Antialiased LineEnable;
			0,											// UINT Forced SampleCount;
			D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF,	// D3D12_CONSERVATIVE_RASTERIZATION_MODE ConservativeRaster;
		};
	} RASTERIZER_STATE;
	const struct
	{
		const D3D12_DEPTH_STENCIL_DESC1 DISABLED
		{
			0,									//BOOL DepthEnable;
			D3D12_DEPTH_WRITE_MASK_ZERO,		//D3D12_DEPTH_WRITE_MASK DepthWriteMask;
			D3D12_COMPARISON_FUNC_LESS_EQUAL,	//D3D12_COMPARISON_FUNC DepthFunc;
			0,									//BOOL StencilEnable;
			0,									//UINT8 Stencil ReadMask;
			0,									//UINT8 StencilWriteMask;
			{},									//D3D12_DEPTH_STENCILOP_DESC FrontFace;
			{},									//D3D12_DEPTH_STENCILOP_DESC BackFace;
			0									//BOOL DepthBounds TestEnable;
		};
	} DEPTH_STATE;

	const struct
	{
		const D3D12_HEAP_PROPERTIES DEFAULT
		{
			D3D12_HEAP_TYPE_DEFAULT,
			D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
			D3D12_MEMORY_POOL_UNKNOWN,
			0,
			0
		};

		const D3D12_HEAP_PROPERTIES UPLOAD
		{
			D3D12_HEAP_TYPE_UPLOAD,
			D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
			D3D12_MEMORY_POOL_UNKNOWN,
			0,
			0
		};
	} HEAP;

	extern ID3D12Device8* device;
	extern IDXGIFactory7* dxgiFactory;
	extern IDXGIAdapter4* dxgiAdapter;
	extern CommandQueue g_cmdQueue;
	extern DescriptorHeap g_rtvHeap;
	extern DescriptorHeap g_dsvHeap;
	extern DescriptorHeap g_srvHeap;
	extern DescriptorHeap g_uavHeap;
	
	void InitD3D();																			// initialize D3D
	void ShutdownD3D();																		// shutdown D3D
	void RenderFrame(ID3D12GraphicsCommandList6* cmd, GFX_FRAME_DESC& desc);				// render to the given render target view
}