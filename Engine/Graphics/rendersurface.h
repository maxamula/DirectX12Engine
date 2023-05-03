#pragma once
#include "graphics.h"
#include "overlay.h"
#include "gpass.h"
#include "gresource.h"
#include "postprocess.h"
#include <chrono>

#define MAX_FRAMES 1000

namespace engine
{
	class WindowImpl;
	namespace gfx
	{
		class RenderSurface
		{
		public:
			friend class engine::WindowImpl;
			RenderSurface() = default;
			RenderSurface(const RenderSurface&) = delete;
			RenderSurface& operator=(const RenderSurface&) = delete;
			RenderSurface(RenderSurface&&) = delete;
			RenderSurface& operator=(RenderSurface&&) = delete;
			RenderSurface(HWND hWnd, unsigned short width, unsigned short height);
			~RenderSurface() { assert(m_pSwap == NULL); }

			inline void Present()
			{
				m_pSwap->Present(bVSync, 0);
				m_backBufferIndex = m_pSwap->GetCurrentBackBufferIndex();
			}

			inline D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle(uint32_t index)
			{
				assert_throw(index < BACKBUFFER_COUNT, "Invalid backbuffer index!");
				return m_renderTargets[index].allocation.CPU;
			}

			void Release();
			void Resize(uint16_t width, uint16_t height);
			inline uint16_t GetWidth() const { return m_width; }
			inline uint16_t GetHeight() const { return m_height; }
			void SetOverlayContext();
			void Render();

			void(_cdecl* cbOverlay)() = nullptr;
			uint8_t bVSync = 1;
		protected:
			void _CreateRendertargetViews();
#ifdef _DEBUG_GRAPHICS
			void _DrawDebugInfo();
#endif

			IDXGISwapChain4* m_pSwap = nullptr;
			D3D12_VIEWPORT m_viewport{};
			D3D12_RECT m_scissiors{};
			RENDER_TARGET m_renderTargets[BACKBUFFER_COUNT]{};
			uint8_t m_backBufferIndex = 0;

			overlay::OVERLAY_CONTEXT* m_overlayContext = nullptr;

			HWND m_hWnd = NULL;
			uint16_t m_width = 0;
			uint16_t m_height = 0;
		};
	}
}

