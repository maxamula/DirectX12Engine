#pragma once
#include "graphics.h"
#include "rendersurface.h"
#include "../window.h"

namespace engine::gfx
{
	class GfxWindow : engine::Window
	{
		friend LRESULT CALLBACK WndProcBase(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	public:
		GfxWindow() = delete;
		GfxWindow(HINSTANCE hInst, GFX_WND_DESC& desc);
		inline void Destroy() { ::DestroyWindow(m_hWnd); };
		void ShowWnd();
		void CloseWnd();
		void SetWindowSize(uint16_t width, uint16_t height);
		void SetWindowCaption(const wchar_t* szCaption);
		void Fullscreen(bool bState);

		bool IsFullscreen() const override { return m_bFullscreen; };
		bool IsClosed() const override { return m_bClosed; }
		uint16_t Width() const override { return m_width; }
		uint16_t Height() const override { return m_height; }
		HWND WinId() const override { return m_hWnd; }
	private:
		void _UpdateSize();

		RenderSurface m_surface;

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
		bool m_bFullscreen = false;
		bool m_bClosed = true;
	};

	LRESULT CALLBACK WndProcBase(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		GfxWindow* This = (GfxWindow*)GetWindowLongPtr(hwnd, 0);
		switch (msg)
		{
		case WM_CLOSE:
			This->CloseWnd();
			break;
		case WM_DESTROY:
			This->m_surface.Release();
			delete This;
			return DefWindowProc(hwnd, msg, wparam, lparam);
			break;
		case WM_NCDESTROY:
			return DefWindowProc(hwnd, msg, wparam, lparam);
			break;
		case WM_EXITSIZEMOVE:
			GetClientRect(hwnd, &This->m_clientRect);
			break;
		case WM_SIZE:	// TODO resize surface
			if (wparam == SIZE_MAXIMIZED)
				GetClientRect(hwnd, &This->m_clientRect);
			break;
		case WM_SYSCOMMAND:
			if (wparam == SC_RESTORE)
				GetClientRect(hwnd, &This->m_clientRect);
			break;
		case WM_QUIT:
			This->Destroy();
			break;
		}
		return (This && This->m_callback) ? This->m_callback(This, hwnd, msg, wparam, lparam) : DefWindowProc(hwnd, msg, wparam, lparam);
	}

}
