#include "window.h"
#include <imgui.h>
#include <ImGuizmo.h>

#include <thread>

namespace Engine
{
	float cameraView[16] =
	{ 1.f, 0.f, 0.f, 0.f,
	  0.f, 1.f, 0.f, 0.f,
	  0.f, 0.f, 1.f, 0.f,
	  0.f, 0.f, 0.f, 1.f };

	float mat[16] =
	{ 1.f, 0.f, 0.f, 0.f,
	  0.f, 1.f, 0.f, 0.f,
	  0.f, 0.f, 1.f, 0.f,
	  0.f, 0.f, 0.f, 1.f };

	float cameraProjection[16];

	void Overlay();

	static public ref class Viewport
	{
	internal:
		static ImGuiContext* s_ctx = nullptr;
		static engine::Window* s_window = nullptr;
	public:
		static System::IntPtr SetParent(System::IntPtr hParent)
		{
			engine::GFX_WND_DESC desc{};
			desc.callback = nullptr;
			desc.hParent = (HWND)hParent.ToPointer();
			desc.width = 10;
			desc.height = 10;
			s_window = engine::Window::Create(GetModuleHandle(NULL), desc);
			s_ctx = (ImGuiContext*)s_window->GetOverlayContext();
			s_window->SetOverlay(Overlay);
			s_window->VSync(false);
			return System::IntPtr(s_window->WinId());
		}

		static void Destroy()
		{
			s_window->Destroy();
		}

		static void Render()
		{
			s_window->Render();	
		}

		static void Resize(uint16_t width, uint16_t height)
		{
			s_window->SetWindowSize(width, height);
		}
	};

	public ref class Window
	{
	internal:
		static ImGuiContext* s_ctx = nullptr;
		engine::Window* m_window = nullptr;
	public:
		// As child
		Window(System::IntPtr hParent)
		{
			engine::GFX_WND_DESC desc{};
			desc.callback = nullptr;
			desc.hParent = (HWND)hParent.ToPointer();
			desc.width = 10;
			desc.height = 10;
			m_window = engine::Window::Create(GetModuleHandle(NULL), desc);
			s_ctx = (ImGuiContext*)m_window->GetOverlayContext();
			m_window->SetOverlay(Overlay);
		}

		~Window()
		{
		}

		void Resize(uint16_t width, uint16_t height)
		{
			m_window->SetWindowSize(width, height);
		}

		void Render()
		{
			m_window->Render();
		}

		void Destroy()
		{
			m_window->Destroy();
		}

		System::IntPtr GetHandle()
		{
			return System::IntPtr(m_window->WinId());
		}

	private:
		!Window()
		{
		}
		void _Render()
		{
			while (m_bRunning)
			{
				m_window->Render();
			}
		}
		bool m_bRunning = true;
		System::Threading::Thread^ m_renderThread;
	};

	void Overlay()
	{
		ImGui::SetCurrentContext(Viewport::s_ctx);
		ImGuiIO& io = ImGui::GetIO();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		float viewManipulateRight = io.DisplaySize.x - 128;
		float viewManipulateTop = io.DisplaySize.y;

		ImGuizmo::BeginFrame();
		ImGuizmo::Enable(true);
		ImGuizmo::ViewManipulate(cameraView, 12, ImVec2(viewManipulateRight, 0), ImVec2(128, 128), 0x10101010);
	}
}