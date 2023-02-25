#include "gfxwindow.h"

namespace engine::gfx
{
	GfxWindow::GfxWindow(HINSTANCE hInst, GFX_WND_DESC& desc)
		: m_hInst(hInst), m_parent(desc.hParent), m_callback(desc.callback), m_caption(desc.szCaption), m_width(desc.width), m_height(desc.height)
	{
		RECT rect{ desc.left, desc.top, desc.left + desc.width, desc.top + desc.height };
		m_dwStyle = desc.hParent != NULL ? WS_CHILD : WS_OVERLAPPEDWINDOW;
		AdjustWindowRect(&rect, m_dwStyle, FALSE);
		m_hWnd = CreateWindow(WND_CLASS, m_caption.c_str(), m_dwStyle, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, m_parent, NULL, hInst, NULL);
		GetWindowRect(m_hWnd, &m_wndRect);
		GetClientRect(m_hWnd, &m_clientRect);
		SetWindowLongPtr(m_hWnd, 0, (LONG_PTR)this);

		// create rtv && swapchain
		new (&m_surface) gfx::RenderSurface(m_hWnd, m_width, m_height);
	}

	void GfxWindow::ShowWnd()
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
	}

	void GfxWindow::CloseWnd()
	{
		m_bClosed = true;
		::CloseWindow(m_hWnd);
	}

	void GfxWindow::SetWindowSize(uint16_t width, uint16_t height)
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

	void GfxWindow::SetWindowCaption(const wchar_t* szCaption)
	{
		SetWindowText(m_hWnd, szCaption);
		m_caption = std::wstring(szCaption);
	}

	void GfxWindow::Fullscreen(bool bState)
	{

	}
}