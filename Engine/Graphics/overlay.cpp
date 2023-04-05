#include "overlay.h"
#include "descriptorheap.h"

namespace engine::gfx::overlay
{
    static std::unordered_map<ImGuiContext*, gfx::DESCRIPTOR_HANDLE> s_imguiHeaps;

    ImGuiContext* Initialize(HWND hWnd, uint16_t initWidth, uint16_t initHeight)
    {
        LOG_DEBUG("Initializing ImGui..."); 
        ImGuiContext* context = ImGui::CreateContext();
        ImGui::SetCurrentContext(context);
        assert_throw(context, "Failed to create ImGui context.");
        assert(s_imguiHeaps.find(context) == s_imguiHeaps.end());
        //ImGui::StyleColorsLight();
        ImGui::StyleColorsClassic();

        gfx::DESCRIPTOR_HANDLE allocation = g_srvHeap.Allocate();

        ImGui_ImplWin32_Init(hWnd);
        ImGui_ImplDX12_Init(device, BACKBUFFER_COUNT, DXGI_FORMAT_R8G8B8A8_UNORM, g_srvHeap.GetDescriptorHeap(), allocation.CPU, allocation.GPU);

        s_imguiHeaps[context] = allocation;

        return context;
    }

    void Shutdown(ImGuiContext* context)
    {
        if (!context) return;
        auto it = s_imguiHeaps.find(context);
        assert_throw(it != s_imguiHeaps.end(), "Overlay release error. ImGui context not found.");
        
        ImGui::SetCurrentContext(context);
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
        g_srvHeap.Free(it->second);
        s_imguiHeaps.erase(it);
	}

}
