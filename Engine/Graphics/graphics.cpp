#include "graphics.h"

namespace engine::gfx
{
	// Global decl
	// direct3d stuff
	ID3D12Device8* device = nullptr;
	IDXGIFactory7* dxgiFactory = nullptr;
	IDXGIAdapter4* dxgiAdapter = nullptr;

	CommandQueue g_cmdQueue;

	DescriptorHeap g_rtvHeap;
	DescriptorHeap g_dsvHeap;
	DescriptorHeap g_srvHeap;
	DescriptorHeap g_uavHeap;


	void InitD3D()
	{
		// enable debug layer if in debug mode
		#if defined(_DEBUG)
		{
			ID3D12Debug* debugController;
			D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
			debugController->EnableDebugLayer();
		}
		#endif
		// Shutdown if initialize
		if (device) ShutdownD3D();
		// Select adapter
		{
			// Create factory
			CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory));
			// Enumerate adapters
			IDXGIAdapter1* adapter1 = nullptr;
			for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &adapter1) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				// Check if adapter is compatible
				DXGI_ADAPTER_DESC1 desc;
				adapter1->GetDesc1(&desc);
				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					continue;
				// Check if adapter supports d3d12
				if (SUCCEEDED(D3D12CreateDevice(adapter1, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))))
				{
					// Get adapter
					adapter1->QueryInterface(IID_PPV_ARGS(&dxgiAdapter));
					break;
				}
			}
		}
		device->SetName(L"MAIN");

		new (&g_cmdQueue) CommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		new (&g_rtvHeap) DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 512);
		new (&g_dsvHeap) DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 512);
		new (&g_srvHeap) DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 4096);
		new (&g_uavHeap) DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 512);

		shaders::Initialize();
		gpass::Initialize();
	}

	void ShutdownD3D()
	{
		// Release all resources
		gpass::Shutdown();
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