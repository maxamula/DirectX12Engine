#include "rendersurface.h"
#include "gpass.h"
#include "gresource.h"
#include "postprocess.h"

namespace engine::gfx
{
	RenderSurface::RenderSurface(HWND hWnd, uint16_t width, uint16_t height)
		: m_hWnd(hWnd), m_width(width), m_height(height), m_viewport({ 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f }), m_scissiors({ 0, 0, (LONG)width, (LONG)height })
	{
		assert(hWnd != NULL && hWnd != INVALID_HANDLE_VALUE);
		Release();
		DXGI_SWAP_CHAIN_DESC1 scDesc = {};
		scDesc.Width = width;
		scDesc.Height = height;
		scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scDesc.Stereo = false;
		scDesc.SampleDesc.Count = 1;
		scDesc.SampleDesc.Quality = 0;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.BufferCount = BACKBUFFER_COUNT;
		scDesc.Scaling = DXGI_SCALING_STRETCH;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

		IDXGISwapChain1* pSwap = NULL;
		HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(g_cmdQueue.GetCommandQueue(), hWnd, &scDesc, NULL, NULL, &pSwap);
		assert(SUCCEEDED(hr));
		hr = pSwap->QueryInterface(__uuidof(IDXGISwapChain4), (void**)&m_pSwap);
		assert(SUCCEEDED(hr));
		m_backBufferIndex = m_pSwap->GetCurrentBackBufferIndex();
		pSwap->Release();

		// Overlay initialization
		m_overlayContext = overlay::Initialize(hWnd, width, height);
#ifdef _DEBUG_GRAPHICS

#endif
		_CreateRendertargetViews();
	}

	void RenderSurface::Release()
	{
		g_cmdQueue.Flush();
		overlay::Shutdown(m_overlayContext);		
		for (int i = 0; i < BACKBUFFER_COUNT; ++i)
		{
			RELEASE(m_renderTargets[i].resource);
			g_rtvHeap.Free(m_renderTargets[i].allocation);
			m_renderTargets[i].allocation = {};
		}
		RELEASE(m_pSwap);
	}

	void RenderSurface::SetOverlayContext()
	{
		ImGui::SetCurrentContext(m_overlayContext->imguiContext);
		ImPlot::SetCurrentContext(m_overlayContext->implotContext);
	}

	void RenderSurface::Render()
	{
		g_cmdQueue.BeginFrame();
		ID3D12GraphicsCommandList6* cmd = g_cmdQueue.GetCommandList();
		// Get back buffer
		ID3D12Resource* backBuffer = m_renderTargets[m_backBufferIndex].resource;

		GFX_FRAME_DESC desc
		{
			m_width,
			m_height
		};
		

		if (GetAsyncKeyState(VK_F1))
			Sleep(100);

		// Record commands
		cmd->RSSetViewports(1, &m_viewport);
		cmd->RSSetScissorRects(1, &m_scissiors);
		ID3D12DescriptorHeap* heaps[] = { g_srvHeap.GetDescriptorHeap() };
		cmd->SetDescriptorHeaps(_countof(heaps), heaps);
		g_resourceBarriers.Add(backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

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
		fx::PostProcess(cmd, m_renderTargets[m_backBufferIndex].allocation.CPU);
#pragma endregion

#pragma region Overlay
		// Start the Dear ImGui frame
		SetOverlayContext();
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		// Draw overlay
#ifdef _DEBUG_GRAPHICS
		_DrawDebugInfo();
#endif
		if(cbOverlay)
			cbOverlay();
		ImGui::Render();
		// Render Dear ImGui graphics		
		cmd->OMSetRenderTargets(1, &m_renderTargets[m_backBufferIndex].allocation.CPU, FALSE, NULL);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmd);
#pragma endregion
		TransitionResource(cmd, backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		g_cmdQueue.EndFrame();
		Present();
	}

	void RenderSurface::Resize(uint16_t width, uint16_t height)
	{
		g_cmdQueue.Flush();
		assert(m_pSwap != NULL);
		m_width = width;
		m_height = height;
		m_viewport.Width = (float)width;
		m_viewport.Height = (float)height;
		m_scissiors.right = (LONG)width;
		m_scissiors.bottom = (LONG)height;
		for (int i = 0; i < BACKBUFFER_COUNT; ++i)
		{
			RELEASE(m_renderTargets[i].resource);
		}
		succeed(m_pSwap->ResizeBuffers(BACKBUFFER_COUNT, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0), "Failed to resize swap chain.");
		m_backBufferIndex = m_pSwap->GetCurrentBackBufferIndex();

		for (int i = 0; i < BACKBUFFER_COUNT; ++i)
		{
			succeed(m_pSwap->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_renderTargets[i].resource), "Failed to get back buffer.");
			wchar_t name[17];
			swprintf_s(name, L"Backbuffer (%d)", i);
			SET_NAME(m_renderTargets[i].resource, name);
			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
			rtvDesc.Texture2D.PlaneSlice = 0;
			device->CreateRenderTargetView(m_renderTargets[i].resource, &rtvDesc, m_renderTargets[i].allocation.CPU);
		}
		LOG_TRACE("D3D12 Surface resized.");
		// TODO
		OutputDebugString(L"D3D12 Surface resized.\n");
	}

	void RenderSurface::_CreateRendertargetViews()
	{
		assert(m_pSwap != NULL);
		for (int i = 0; i < BACKBUFFER_COUNT; ++i)
		{
			succeed(m_pSwap->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_renderTargets[i].resource), "Failed to get backbuffer.");
			wchar_t name[17];
			swprintf_s(name, L"Backbuffer (%d)", i);
			SET_NAME(m_renderTargets[i].resource, name);
			m_renderTargets[i].allocation = g_rtvHeap.Allocate();
			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
			rtvDesc.Texture2D.PlaneSlice = 0;
			device->CreateRenderTargetView(m_renderTargets[i].resource, &rtvDesc, m_renderTargets[i].allocation.CPU);
		}
	}

#ifdef _DEBUG_GRAPHICS
	void RenderSurface::_DrawDebugInfo()
	{

		ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus;


		// Calculate the window size and position based on the available screen space
		const ImVec2 screenSize = ImGui::GetIO().DisplaySize;
		const ImVec2 windowSize = ImVec2(screenSize.x > 450 ? 350 : screenSize.x * 0.5f, screenSize.y);
		const ImVec2 windowPos = ImVec2(0, 0);

		ImGui::SetNextWindowSize(windowSize);
		ImGui::SetNextWindowPos(windowPos);
		ImGui::Begin("Frame", nullptr, windowFlags);

		ImGui::Text("D3D12 Device: %p", device);
		ImGui::Text("D3D12 Swapchain: %p", m_pSwap);
		ImGui::Checkbox("VSync", (bool*)&bVSync);
		gpass::DrawDebugInfo(m_width, m_height);
		fx::DrawDebugInfo(m_width, m_height);
		ImGui::End();
	}
#endif
}