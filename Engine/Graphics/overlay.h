#pragma once
#include "Graphics/graphics.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include <implot.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace engine::gfx::overlay
{
    struct OVERLAY_CONTEXT
    {
        ImGuiContext* imguiContext;
        ImPlotContext* implotContext;
    };

	OVERLAY_CONTEXT* Initialize(HWND hWnd, uint16_t initWidth, uint16_t initHeight);
	void Shutdown(OVERLAY_CONTEXT* context);
}