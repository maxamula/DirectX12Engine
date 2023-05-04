#include "renderer.h"
#include "gpass.h"
#include "postprocess.h"

namespace engine::gfx
{
	void RenderFrame(GFX_FRAME_RENDER_TARGET_DESC& desc)
	{
		// REMOVE LATER!!
		GFX_FRAME_DESC fdesc{};
		//---------------
		ID3D12GraphicsCommandList6* cmd = g_cmdQueue.GetCommandList();
		cmd->RSSetViewports(desc.rtvCount, desc.viewports);
		cmd->RSSetScissorRects(desc.rtvCount, desc.scissiors);
		ID3D12DescriptorHeap* heaps[] = { g_srvHeap.GetDescriptorHeap() };
		cmd->SetDescriptorHeaps(_countof(heaps), heaps);
		for (uint32_t i = 0; i < desc.rtvCount; i++)
		{
			g_resourceBarriers.Add(desc.renderTargets[i].resource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		}

#pragma region Depth prepass
		// Resource transition
		g_resourceBarriers.Add(gpass::GetDepthBuffer().Resource(), D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_DEPTH_WRITE);
		g_resourceBarriers.Flush(cmd);
		gpass::SetPipelineForDepthPrepass(cmd);
		gpass::DepthPrepass(cmd, fdesc);
#pragma endregion

#pragma region GPass
		// Resource transition
		g_resourceBarriers.Add(gpass::GetMainBuffer().Resource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
		g_resourceBarriers.Add(gpass::GetDepthBuffer().Resource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		g_resourceBarriers.Flush(cmd);
		gpass::SetPipelineForGPass(cmd);
		gpass::Render(cmd, fdesc);
#pragma endregion

#pragma region Post process
		g_resourceBarriers.Add(gpass::GetMainBuffer().Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		g_resourceBarriers.Flush(cmd);
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvs;
		rtvs.reserve(desc.rtvCount);
		for (uint32_t i = 0; i < desc.rtvCount; i++)
		{
			rtvs.emplace_back(desc.renderTargets[i].allocation.CPU);
		}
		cmd->OMSetRenderTargets(desc.rtvCount, rtvs.data(), 1, nullptr);
		fx::PostProcess(cmd);
#pragma endregion

		for (uint32_t i = 0; i < desc.rtvCount; i++)
		{
			g_resourceBarriers.Add(desc.renderTargets[i].resource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		}
		g_resourceBarriers.Flush(cmd);
	}
}