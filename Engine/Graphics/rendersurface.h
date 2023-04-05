#pragma once
#include "graphics.h"
#include "overlay.h"

#include <chrono>

namespace engine::gfx
{
	struct RENDER_TARGET
	{
		ID3D12Resource* resource = nullptr;
		DESCRIPTOR_HANDLE allocation = {};
	};

	class RenderSurface
	{
	public:
		RenderSurface() = default;
		RenderSurface(const RenderSurface&) = delete;
		RenderSurface& operator=(const RenderSurface&) = delete;
		RenderSurface(RenderSurface&&) = delete;
		RenderSurface& operator=(RenderSurface&&) = delete;
		RenderSurface(HWND hWnd, unsigned short width, unsigned short height);
		~RenderSurface() { assert(m_pSwap == NULL); };

		inline void Present()
		{
			m_pSwap->Present(1, 0);
			m_backBufferIndex = m_pSwap->GetCurrentBackBufferIndex();
		}

		inline D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle(uint32_t index)
		{
			assert(index < BACKBUFFER_COUNT);
			return m_renderTargets[index].allocation.CPU;
		}

		void Release();
		void Resize(uint16_t width, uint16_t height);
		void SetImGuiContext();
		void Render();
	private:
		void _CreateRendertargetViews();
#ifdef _DEBUG_GRAPHICS
		void _DrawDebugInfo();
#endif

		IDXGISwapChain4* m_pSwap = nullptr;
		D3D12_VIEWPORT m_viewport{};
		D3D12_RECT m_scissiors{};
		RENDER_TARGET m_renderTargets[BACKBUFFER_COUNT]{};
		byte m_backBufferIndex = 0;

		ImGuiContext* m_pImGuiContext = nullptr;

		HWND m_hWnd = NULL;
		uint16_t m_width = 0;
		uint16_t m_height = 0;

		// Debug/Intermediate info
		float m_frameTime = 0.0f;
	};
}

