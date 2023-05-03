#include "editor.h"
#include "common.h"
#include "engine.h"
#include "Graphics/rendersurface.h"

class CustomSurface;

using namespace engine;
using namespace engine::gfx;
CustomSurface* g_surface;
RECT g_sizingRect = {};
bool g_bWasRestored = false;

class CustomSurface : public RenderSurface
{
public:
	CustomSurface() = default;
	CustomSurface(const CustomSurface&) = delete;
	CustomSurface& operator=(const CustomSurface&) = delete;
	CustomSurface(CustomSurface&&) = delete;
	CustomSurface& operator=(CustomSurface&&) = delete;
	CustomSurface(HWND hWnd, unsigned short width, unsigned short height)
		: RenderSurface(hWnd, width, height) {}

	void Render() override
	{
		ID3D12GraphicsCommandList6* cmd = g_cmdQueue.GetCommandList();
		GFX_FRAME_DESC desc;
		desc.renderTargets = &m_renderTargets[m_backBufferIndex];
		desc.scissiors = &m_scissiors;
		desc.viewports = &m_viewport;
		desc.rtvCount = 1;
		desc.surfHeight = m_height;
		desc.surfWidth = m_width;
		g_cmdQueue.BeginFrame();
#pragma region Overlay
		// resource transition stuff
		TransitionResource(cmd, m_renderTargets[m_backBufferIndex].resource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		// Start the Dear ImGui frame
		SetOverlayContext();
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
		ImGui::Begin("Frame", nullptr, windowFlags);

		ImGui::Text("D3D12 Device: %p", device);
		ImGui::Text("D3D12 Device: %p", device);
		ImGui::Text("D3D12 Device: %p", device);
		ImGui::Text("D3D12 Device: %p", device);
		ImGui::Text("D3D12 Device: %p", device);
		ImGui::Text("D3D12 Device: %p", device);
		ImGui::Text("D3D12 Device: %p", device);
		ImGui::End();
#endif
		ImGui::Render();
		// Render Dear ImGui graphics
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmd);
		// Transition back
		TransitionResource(cmd, m_renderTargets[m_backBufferIndex].resource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
#pragma endregion
		g_cmdQueue.EndFrame();
		Present();
	}
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CLOSE:
		if (MessageBox(NULL, L"Are you sure?", L"Exit", MB_OKCANCEL) == IDOK)
			PostQuitMessage(1);
		return 0;
		break;
	case WM_DESTROY:
		g_surface->Release();
		delete g_surface;
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
		if (g_sizingRect.right != g_surface->GetWidth() || g_sizingRect.bottom != g_surface->GetHeight())
		{
			g_surface->Resize(g_sizingRect.right - g_sizingRect.left, g_sizingRect.bottom - g_sizingRect.top);
		}
		break;
	case WM_SIZE:	// TODO resize surface
		if (wparam == SIZE_MAXIMIZED || g_bWasRestored)
		{
			GetClientRect(hwnd, &g_sizingRect);
			g_surface->Resize(g_sizingRect.right - g_sizingRect.left, g_sizingRect.bottom - g_sizingRect.top);
			g_bWasRestored = false;
		}
		break;
	case WM_SYSCOMMAND:
		if (wparam == SC_RESTORE)
			g_bWasRestored = true;
		break;
	case WM_QUIT:
		return DefWindowProc(hwnd, msg, wparam, lparam);
		break;
	}
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
		return true;

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{
	engine::Init();
	// Register win32 window class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"Editor";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_APPLICATION);
	RegisterClassEx(&wcex);

	// Create window
	HWND hWnd = CreateWindow(L"Editor", L"Editor", WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	// Show window
	ShowWindow(hWnd, nCmdShow);
	g_surface = new CustomSurface(hWnd, 1280, 720);

	// Msg loop
	MSG msg;
	while (true)
	{
		PeekMessage(&msg, hWnd, 0u, 0u, PM_REMOVE);
		if(msg.message == WM_QUIT)
				break;
		g_surface->Render();
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	g_surface->Release();
	engine::Shutdown();
}