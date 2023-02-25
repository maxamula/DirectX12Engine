#include "rendersurface.h"

namespace engine::gfx
{
	RenderSurface::RenderSurface(HWND hWnd, unsigned short width, unsigned short height)
		: m_hWnd(hWnd), m_width(width), m_height(height), m_viewport({ 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f })
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
		_CreateRendertargetViews();
	}

	void RenderSurface::Release()
	{
		if (m_pSwap)
		{
			for (int i = 0; i < BACKBUFFER_COUNT; ++i)
			{
				RELEASE(m_renderTargets[i].resource);
				g_rtvHeap.Free(m_renderTargets[i].allocation);
				m_renderTargets[i].allocation = {};
			}
			RELEASE(m_pSwap);
		}
	}

	void RenderSurface::Resize(unsigned short width, unsigned short height)
	{
		assert(m_pSwap != NULL);
		m_width = width;
		m_height = height;
		m_viewport.Width = (float)width;
		m_viewport.Height = (float)height;
		Release();
		HRESULT hr = m_pSwap->ResizeBuffers(BACKBUFFER_COUNT, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		assert(SUCCEEDED(hr));
		m_backBufferIndex = m_pSwap->GetCurrentBackBufferIndex();
		_CreateRendertargetViews();
	}

	void RenderSurface::_CreateRendertargetViews()
	{
		assert(m_pSwap != NULL);
		for (int i = 0; i < BACKBUFFER_COUNT; ++i)
		{
			HRESULT hr = m_pSwap->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_renderTargets[i].resource);
			assert(SUCCEEDED(hr));
			m_renderTargets[i].allocation = g_rtvHeap.Allocate();
			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
			rtvDesc.Texture2D.PlaneSlice = 0;
			device->CreateRenderTargetView(m_renderTargets[i].resource, &rtvDesc, m_renderTargets[i].allocation.CPU);
		}
	}
}