#pragma once
#include "Graphics/graphics.h"
#include "../Dependencies/imgui.h"
#include "../Dependencies/imgui_impl_dx12.h"
#include "../Dependencies/imgui_impl_win32.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace engine::gfx::overlay
{
	ImGuiContext* Initialize(HWND hWnd, uint16_t initWidth, uint16_t initHeight);
	void Shutdown(ImGuiContext* context);
}