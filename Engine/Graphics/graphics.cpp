#include "graphics.h"
#include "shaders.h"
#include "gpass.h"
#include "postprocess.h"
#include "overlay.h"

namespace engine::gfx
{
	// Global decl
	// direct3d stuff
	ID3D12Device8* device = nullptr;
	IDXGIFactory7* dxgiFactory = nullptr;
	IDXGIAdapter4* dxgiAdapter = nullptr;
	DXGI_ADAPTER_DESC1 g_adapterDesc{};

	CommandQueue g_cmdQueue;

	DescriptorHeap g_rtvHeap;
	DescriptorHeap g_dsvHeap;
	DescriptorHeap g_srvHeap;
	DescriptorHeap g_uavHeap;


	void InitD3D()
	{
		LOG_DEBUG("Initializing graphics...");
		// enable debug layer if in debug mode
		#if defined(_DEBUG)
		{
			ID3D12Debug* debugController;
			D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
			debugController->EnableDebugLayer();
			LOG_TRACE("Debug layer enabled");
		}
		#endif
		// Shutdown if initialized
		if (device)
		{
			LOG_WARN("Graphics already initialized, shutting down...");
			ShutdownD3D();
		}
		// Select adapter
		{
			// Create factory
			CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory));
			// Enumerate adapters
			IDXGIAdapter1* adapter1 = nullptr;
			for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &adapter1) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				// Check if adapter is compatible
				adapter1->GetDesc1(&g_adapterDesc);
				if (g_adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					continue;
				// Check if adapter supports d3d12
				if (SUCCEEDED(D3D12CreateDevice(adapter1, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))))
				{
					// Get adapter
					adapter1->QueryInterface(IID_PPV_ARGS(&dxgiAdapter));
					size_t len = wcslen(g_adapterDesc.Description) + 1;
					char* cstr = new char[len];
					wcstombs(cstr, g_adapterDesc.Description, len);
					LOG_INFO("Suitable adapter found: {:<30}", cstr);
					delete cstr;
					//break;
				}
			}
		}
		SET_NAME(device, L"Device");

		new (&g_cmdQueue) CommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		new (&g_rtvHeap) DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 512);
		new (&g_dsvHeap) DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 512);
		new (&g_srvHeap) DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 4096);
		new (&g_uavHeap) DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 512);

		LOG_DEBUG("Compiling shaders...");
		if(!shaders::Initialize()) { LOG_CRITICAL("Failed to initialize shaders"); }
		LOG_DEBUG("Initializing gpass...");
		if (!gpass::Initialize()) { LOG_CRITICAL("Failed to initialize GPAss"); }
		LOG_DEBUG("Initializing fx...");
		if (!fx::Initialize()) { LOG_CRITICAL("Failed to initialize FX"); }

		IMGUI_CHECKVERSION();
		LOG_INFO("Graphics initialized");
	}

	void ShutdownD3D()
	{
		LOG_DEBUG("Graphics shutdown...");
		// Release all resources
		gpass::Shutdown();
		fx::Shutdown();
		shaders::Shutdown();
		RELEASE(device);
		RELEASE(dxgiFactory);
		RELEASE(dxgiAdapter);
		g_cmdQueue.Release();
		g_rtvHeap.Release();
		g_dsvHeap.Release();
		g_srvHeap.Release();
		g_uavHeap.Release();
	}
}