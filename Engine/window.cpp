#include "window.h"
#include "Graphics/graphics.h"
#include "Graphics/rendersurface.h"
#include "Graphics/overlay.h"
#include "Graphics/gpass.h"
#include "scenegraph.h"


namespace engine
{
	class WindowImpl;
	namespace
	{
		
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
				GetClientRect(m_hWnd, &m_clientRect);
				return;
			}
			m_width = width;
			m_height = height;
			m_wndRect.right += width - m_wndRect.right;
			m_wndRect.bottom += height - m_wndRect.bottom;
			AdjustWindowRect(&m_wndRect, m_dwStyle, FALSE);
			MoveWindow(m_hWnd, m_wndRect.left, m_wndRect.top, m_wndRect.right, m_wndRect.bottom, true);

			GetClientRect(m_hWnd, &m_clientRect);
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

		void Render() override
		{
			m_surface.Render();
		}

		bool IsFullscreen() const override { return m_bFullscreen; };
		bool IsClosed() const override { return m_bClosed; }
		uint16_t Width() const override { return m_width; }
		uint16_t Height() const override { return m_height; }
		HWND WinId() const override { return m_hWnd; }
	private:
		void _UpdateSize()
		{
			GetWindowRect(m_hWnd, &m_wndRect);
			GetClientRect(m_hWnd, &m_clientRect);
			m_width = (uint16_t)(m_clientRect.right - m_clientRect.left);
			m_height = (uint16_t)(m_clientRect.bottom - m_clientRect.top);
			m_surface.Resize(m_width, m_height);
			gfx::gpass::UpdateSize(m_width, m_height);
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
		bool m_bFullscreen = false;
		bool m_bClosed = true;
		bool m_flagShouldResize = false;
		bool m_flagWasRestored = false;
	};

	Window* Window::Create(HINSTANCE hInst, GFX_WND_DESC& desc)
	{
		WindowImpl* wnd = new WindowImpl(hInst, desc);
		return wnd;
	}

	LRESULT CALLBACK WndProcBase(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		WindowImpl* This = (WindowImpl*)GetWindowLongPtr(hwnd, 0);

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
		case WM_EXITSIZEMOVE:
			if (This->m_flagShouldResize)
			{
				This->_UpdateSize();
				This->m_flagShouldResize = false;
			}
			break;
		case WM_SIZE:	// TODO resize surface
			if (wparam == SIZE_MAXIMIZED || This->m_flagWasRestored)
			{
				This->_UpdateSize();
				This->m_flagWasRestored = false;
			}
			else
			{
				This->m_flagShouldResize = true;
			}
			break;
		case WM_SYSCOMMAND:
			if (wparam == SC_RESTORE)
				This->m_flagWasRestored = true;
			break;
		case WM_QUIT:
			return DefWindowProc(hwnd, msg, wparam, lparam);
			break;
		}

		if (This)
		{
			This->m_surface.SetImGuiContext();
			if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
				return true;
		}
		
		return (This && This->m_callback) ? This->m_callback(This, hwnd, msg, wparam, lparam) : DefWindowProc(hwnd, msg, wparam, lparam);
	}
}