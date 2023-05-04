#include "window.h"
#include "Graphics/graphics.h"
#include "Graphics/rendersurface.h"
#include "Graphics/overlay.h"
#include "Graphics/gpass.h"
#include "Graphics/postprocess.h"
#include "Graphics/gresource.h"
#include "Graphics/renderer.h"
#include "scenegraph.h"
#include <iostream>

#define SC_TITLEBAR_RESTORE 61730

namespace engine
{
	class WindowImpl;
	namespace
	{
		RECT g_sizingRect = {};
	}

	class WindowImpl : public Window
	{
		friend LRESULT CALLBACK WndProcBase(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	public:
		WindowImpl() = delete;
		WindowImpl(HINSTANCE hInst, GFX_WND_DESC& desc)
			: m_hInst(hInst), m_parent(desc.hParent), m_callback(desc.callback), m_caption(desc.szCaption), m_width(desc.width), m_height(desc.height)
		{
			RECT rect{ desc.left, desc.top, desc.left + desc.width, desc.top + desc.height };
			m_dwStyle = desc.hParent != NULL ? WS_CHILD : WS_OVERLAPPEDWINDOW;
			AdjustWindowRect(&rect, m_dwStyle, FALSE);
			m_hWnd = CreateWindow(WND_CLASS, m_caption.c_str(), m_dwStyle, desc.left, desc.top, rect.right - rect.left, rect.bottom - rect.top, m_parent, NULL, hInst, NULL);
			GetWindowRect(m_hWnd, &m_wndRect);
			GetClientRect(m_hWnd, &m_clientRect);
			SetWindowLongPtr(m_hWnd, 0, (LONG_PTR)this);
			
			// create rtv && swapchain
			new (&m_surface) gfx::RenderSurface(m_hWnd, 100, 100);	// Initialize with whatever size ( Will be updated in ShowWnd() )
		}


		void Destroy() override
		{ 
			::DestroyWindow(m_hWnd); 
		};

		void ShowWnd() override
		{
			if (m_bFullscreen)
			{
				GetWindowRect(m_hWnd, &m_wndRect);
				m_dwStyle = 0;
				SetWindowLongPtr(m_hWnd, GWL_STYLE, m_dwStyle);
				ShowWindow(m_hWnd, SW_MAXIMIZE);
			}
			else
			{
				m_dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
				SetWindowLongPtr(m_hWnd, GWL_STYLE, m_dwStyle);
				MoveWindow(m_hWnd, m_wndRect.left, m_wndRect.top, m_wndRect.right, m_wndRect.bottom, true);
				ShowWindow(m_hWnd, SW_SHOWNORMAL);
			}
			_UpdateSize();
		}

		void CloseWnd() override
		{
			m_bClosed = true;
			::CloseWindow(m_hWnd);
		}

		void SetWindowSize(uint16_t width, uint16_t height) override
		{
			if (m_dwStyle & WS_CHILD)
			{
				m_render = false;
				m_mutex.lock();
				m_width = width;
				m_height = height;
				m_surface.Resize(m_width, m_height);
				gfx::gpass::UpdateSize(m_width, m_height);
				m_mutex.unlock();
				m_render = true;
				return;
			}
			m_width = width;
			m_height = height;
			m_wndRect.right += width - m_wndRect.right;
			m_wndRect.bottom += height - m_wndRect.bottom;
			AdjustWindowRect(&m_wndRect, m_dwStyle, FALSE);
			MoveWindow(m_hWnd, m_wndRect.left, m_wndRect.top, m_wndRect.right, m_wndRect.bottom, true);

			_UpdateSize();
		}

		void SetWindowCaption(const wchar_t* szCaption)	override
		{
			SetWindowText(m_hWnd, szCaption);
			m_caption = std::wstring(szCaption);
		}

		void Fullscreen(bool bState) override
		{
			m_bFullscreen = bState;
		}

		void SetOverlay(void* callback) override
		{
			m_surface.cbOverlay = (void(_cdecl*)())callback;
		}

		void* GetOverlayContext() const override
		{
			return m_surface.m_overlayContext->imguiContext;
		}

		void Render() override
		{
			if (m_render)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				auto desc = m_surface.GenerateRTDesc();
				gfx::g_cmdQueue.BeginFrame();
				gfx::RenderFrame(desc);
#pragma region Overlay
				auto cmd = gfx::g_cmdQueue.GetCommandList();
				// resource transition stuff
				gfx::TransitionResource(cmd, desc.renderTargets->resource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
				// Start the Dear ImGui frame
				m_surface.SetOverlayContext();
				ImGui_ImplDX12_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();
				// Draw overlay
#ifdef _DEBUG_GRAPHICS
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
				ImGui::Begin("DEBUG_VIEW", nullptr, windowFlags);

				ImGui::Text("D3D12 Device: %p", gfx::device);
				gfx::gpass::DrawDebugInfo(m_width, m_height);
				gfx::fx::DrawDebugInfo(m_width, m_height);
				ImGui::End();
#endif
				ImGui::Render();
				// Render Dear ImGui graphics
				ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmd);
				// Transition back
				gfx::TransitionResource(cmd, desc.renderTargets->resource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
#pragma endregion
				gfx::g_cmdQueue.EndFrame();
				m_surface.Present();
			}		
		}

		void VSync(bool bState) override
		{
			m_surface.bVSync = bState;
		}

		[[nodiscard]] bool VSync() const override { return m_surface.bVSync; }
		[[nodiscard]] bool IsFullscreen() const override { return m_bFullscreen; };
		[[nodiscard]] bool IsClosed() const override { return m_bClosed; }
		[[nodiscard]] uint16_t Width() const override { return m_width; }
		[[nodiscard]] uint16_t Height() const override { return m_height; }
		[[nodiscard]] HWND WinId() const override { return m_hWnd; }
	private:
		void _UpdateSize()
		{
			m_render = false;
			m_mutex.lock();
			GetWindowRect(m_hWnd, &m_wndRect);
			GetClientRect(m_hWnd, &m_clientRect);
			m_width = (uint16_t)(m_clientRect.right - m_clientRect.left);
			m_height = (uint16_t)(m_clientRect.bottom - m_clientRect.top);
			m_surface.Resize(m_width, m_height);
			gfx::gpass::UpdateSize(m_width, m_height);
			m_mutex.unlock();
			m_render = true;
			Resized(*this);
		}

		gfx::RenderSurface m_surface;

		HWND m_hWnd = NULL;
		HWND m_parent = NULL;
		HINSTANCE m_hInst = NULL;
		WndProcFnPtr m_callback = NULL;
		uint16_t m_width = 1280;
		uint16_t m_height = 720;
		std::wstring m_caption = L"Game";
		RECT m_wndRect{};
		RECT m_clientRect{};
		DWORD m_dwStyle = WS_OVERLAPPED | WS_SYSMENU;
		std::mutex m_mutex;
		std::atomic<bool> m_render = true;
		bool m_bFullscreen = false;
		bool m_bClosed = true;
		bool m_flagWasRestored = false;
		uint8_t m_pad[1] = { 0 };
	};

	Window* Window::Create(HINSTANCE hInst, GFX_WND_DESC& desc)
	{
		WindowImpl* wnd = new WindowImpl(hInst, desc);
		return wnd;
	}

	LRESULT CALLBACK WndProcBase(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		WindowImpl* This = (WindowImpl*)GetWindowLongPtr(hwnd, 0);
		if(!This && !(msg == WM_QUIT))
			return DefWindowProc(hwnd, msg, wparam, lparam);

		switch (msg)
		{
		case WM_DESTROY:
			This->m_surface.Release();
			delete This;
			return DefWindowProc(hwnd, msg, wparam, lparam);
			break;
		case WM_NCDESTROY:
			return DefWindowProc(hwnd, msg, wparam, lparam);
			break;
		case WM_KEYDOWN:
			
			break;
		case WM_EXITSIZEMOVE:
			GetClientRect(hwnd, &g_sizingRect);
			// if width and height the same then it's a move event
			if (g_sizingRect.right != This->m_clientRect.right || g_sizingRect.bottom != This->m_clientRect.bottom)
			{
				This->_UpdateSize();
			}
			break;
		case WM_SIZE:	// TODO resize surface
			if (wparam == SIZE_MAXIMIZED || This->m_flagWasRestored)
			{
				This->_UpdateSize();
				This->m_flagWasRestored = false;
			}
			break;
		case WM_SYSCOMMAND:
			if (wparam == SC_RESTORE || wparam == SC_TITLEBAR_RESTORE)
				This->m_flagWasRestored = true;
			break;
		case WM_QUIT:
			return DefWindowProc(hwnd, msg, wparam, lparam);
			break;
		}

		This->m_surface.SetOverlayContext();
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
			return true;
		
		return (This->m_callback) ? This->m_callback(This, hwnd, msg, wparam, lparam) : DefWindowProc(hwnd, msg, wparam, lparam);
	}
}