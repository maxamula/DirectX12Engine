#include "graphics.h"
#include "shaders.h"
#include "gpass.h"
#include "postprocess.h"
#include "overlay.h"

#include <PrimitiveBatch.h>

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
					break;
				}
			}
		}
		SET_NAME(device, L"Device");

		new (&g_cmdQueue) CommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		new (&g_rtvHeap) DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 512);
		new (&g_dsvHeap) DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 512);
		new (&g_srvHeap) DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 4096);
		new (&g_uavHeap) DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 512);

		try
		{
			LOG_DEBUG("Compiling shaders...");
			shaders::Initialize();
			LOG_DEBUG("Initializing gpass...");
			gpass::Initialize();
			LOG_DEBUG("Initializing fx...");
			fx::Initialize();
		}
		catch (...)
		{
			LOG_CRITICAL("Failed to initialize graphics");
			ShutdownD3D();
			abort();
		}
		
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

	using namespace DirectX;

	void RenderFrame(ID3D12GraphicsCommandList6* cmd, GFX_FRAME_DESC& desc)
	{
		cmd->RSSetViewports(desc.rtvCount, desc.viewports);
		cmd->RSSetScissorRects(desc.rtvCount, desc.scissiors);
		ID3D12DescriptorHeap* heaps[] = { g_srvHeap.GetDescriptorHeap() };
		cmd->SetDescriptorHeaps(_countof(heaps), heaps);
		for (uint32_t i = 0; i < desc.rtvCount; i++)
		{
			g_resourceBarriers.Add(desc.renderTargets[i].resource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		}

#pragma region Depth prepass
		// Resource transition
		g_resourceBarriers.Add(gpass::GetDepthBuffer().Resource(), D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_DEPTH_WRITE);
		g_resourceBarriers.Flush(cmd);
		gpass::SetPipelineForDepthPrepass(cmd);
		gpass::DepthPrepass(cmd, desc);
#pragma endregion

#pragma region GPass
		// Resource transition
		g_resourceBarriers.Add(gpass::GetMainBuffer().Resource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
		g_resourceBarriers.Add(gpass::GetDepthBuffer().Resource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		g_resourceBarriers.Flush(cmd);
		gpass::SetPipelineForGPass(cmd);
		gpass::Render(cmd, desc);
#pragma endregion

#pragma region Post process
		g_resourceBarriers.Add(gpass::GetMainBuffer().Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		g_resourceBarriers.Flush(cmd);
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvs;
		rtvs.reserve(desc.rtvCount);
		for (uint32_t i = 0; i < desc.rtvCount; i++)
		{
			rtvs.emplace_back(desc.renderTargets[i].allocation.CPU);
		}
		cmd->OMSetRenderTargets(desc.rtvCount, rtvs.data(), 1, nullptr);
		fx::PostProcess(cmd);
#pragma endregion

		for (uint32_t i = 0; i < desc.rtvCount; i++)
		{
			g_resourceBarriers.Add(desc.renderTargets[i].resource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		}
		g_resourceBarriers.Flush(cmd);
	}
}