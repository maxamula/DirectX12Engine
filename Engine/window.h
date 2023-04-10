#pragma once

namespace engine
{
	typedef LRESULT(__stdcall* WndProcFnPtr)(void* This, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	struct GFX_WND_DESC
	{
		WndProcFnPtr callback = NULL;
		HWND hParent = NULL;
		wchar_t* szCaption = (wchar_t*)L"Game";
		uint32_t left = CW_USEDEFAULT;
		uint32_t top = CW_USEDEFAULT;
		uint16_t width = 1280;
		uint16_t height = 720;
	};

	// Window interface
	class ENGINE_API Window
	{
	public:
		static Window* Create(HINSTANCE hInst, GFX_WND_DESC& desc);

		virtual void Destroy() = 0;
		virtual void ShowWnd() = 0;
		virtual void CloseWnd() = 0;
		virtual void SetWindowSize(uint16_t width, uint16_t height) = 0;
		virtual void SetWindowCaption(const wchar_t* szCaption) = 0;
		virtual void Fullscreen(bool bState) = 0;

		virtual void SetOverlay(void* callback) = 0;
		virtual void Render() = 0;

		[[nodiscard]] virtual void* GetOverlayContext() const = 0;
		[[nodiscard]] virtual bool IsFullscreen() const = 0;
		[[nodiscard]] virtual bool IsClosed() const = 0;
		[[nodiscard]] virtual uint16_t Width() const = 0;
		[[nodiscard]] virtual uint16_t Height() const = 0;
		[[nodiscard]] virtual HWND WinId() const = 0;
	};
}

