#include "overlay.h"
#include "descriptorheap.h"

namespace engine::gfx::overlay
{
    static std::unordered_map<OVERLAY_CONTEXT*, gfx::DESCRIPTOR_HANDLE> s_imguiHeaps;

    OVERLAY_CONTEXT* Initialize(HWND hWnd, uint16_t initWidth, uint16_t initHeight)
    {
        LOG_DEBUG("Initializing ImGui..."); 
        ImGuiContext* imguiContext = ImGui::CreateContext();
        ImPlotContext* implotContext = ImPlot::CreateContext();
        ImGui::SetCurrentContext(imguiContext);
        ImPlot::SetCurrentContext(implotContext);
        ImPlot::SetImGuiContext(imguiContext);
        assert_throw(imguiContext, "Failed to create ImGui context.");
        assert_throw(implotContext, "Failed to create ImPlot context.");
        
        OVERLAY_CONTEXT* context = new OVERLAY_CONTEXT{ imguiContext, implotContext };
        assert(s_imguiHeaps.find(context) == s_imguiHeaps.end());

        ImGui::StyleColorsDark();

        gfx::DESCRIPTOR_HANDLE allocation = g_srvHeap.Allocate();

        ImGui_ImplWin32_Init(hWnd);
        ImGui_ImplDX12_Init(device, BACKBUFFER_COUNT, DXGI_FORMAT_R8G8B8A8_UNORM, g_srvHeap.GetDescriptorHeap(), allocation.CPU, allocation.GPU);

        s_imguiHeaps[context] = allocation;

        return context;
    }

    void Shutdown(OVERLAY_CONTEXT* context)
    {
        if (!context) return;
        auto it = s_imguiHeaps.find(context);
        assert_throw(it != s_imguiHeaps.end(), "Overlay release error. ImGui context not found.");
        
        ImGui::SetCurrentContext(context->imguiContext);
        ImPlot::SetCurrentContext(context->implotContext);
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
        ImPlot::DestroyContext();
		ImGui::DestroyContext();
        g_srvHeap.Free(it->second);
        s_imguiHeaps.erase(it);
        delete context;
	}

}
