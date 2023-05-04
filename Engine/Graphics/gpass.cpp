#include "gpass.h"
#include "overlay.h"

#if _DEBUG
constexpr float CLEAR_COLOR[4]{ 0.3f, 0.0f, 0.0f, 0.0f };
#else
constexpr float CLEAR_COLOR[4]{ };
#endif

namespace engine::gfx::gpass
{
	namespace
	{
		RenderTexture g_mainBuffer;
		DepthTexture g_depthBuffer;
		UVec2 g_geomBufferSize = { 100, 100 };
		const DXGI_FORMAT g_mainBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		const DXGI_FORMAT g_depthBufferFormat = DXGI_FORMAT_D32_FLOAT;

		ID3D12RootSignature* g_gpassRootsig = nullptr;
		ID3D12PipelineState* g_gpassPso = nullptr;

		// Debug information
#ifdef _DEBUG_GRAPHICS
		float g_dbgGpassfpsHistory[500];
		uint16_t g_dbgGpassfpsIndex = 0;
		float g_dbgGpassTime = 0.0f;
		float g_dbgDepthPrepassTime = 0.0f;
#endif

		void CreateBuffers(UVec2 size)
		{
			g_mainBuffer.Release();
			g_depthBuffer.Release();

			D3D12_RESOURCE_DESC desc = {};
			desc.Alignment = 0;
			desc.DepthOrArraySize = 1;
			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			desc.Format = g_mainBufferFormat;
			desc.Height = size.y;
			desc.Width = size.x;
			desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			desc.MipLevels = 0;
			desc.SampleDesc = { 1, 0 };

			// Create main texture
			{
				TEXTURE_DESC texDesc = {};
				texDesc.pResDesc = &desc;
				texDesc.initialState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
				texDesc.clearValue.Format = desc.Format;
				memcpy(&texDesc.clearValue.Color, &CLEAR_COLOR[0], sizeof(float) * std::size(CLEAR_COLOR));
				g_mainBuffer = RenderTexture(texDesc);
				SET_NAME(g_mainBuffer.Resource(), L"GPass Main Buffer");
			}

			desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			desc.Format = g_depthBufferFormat;
			desc.MipLevels = 1;

			//Create depth texture
			{
				TEXTURE_DESC texDesc = {};
				texDesc.pResDesc = &desc;
				texDesc.initialState = D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
				texDesc.clearValue.Format = desc.Format;
				texDesc.clearValue.DepthStencil.Depth = 0.0f;
				texDesc.clearValue.DepthStencil.Stencil = 0;
				g_depthBuffer = DepthTexture(texDesc);
			}
			assert_throw(g_mainBuffer.Resource() && g_depthBuffer.Resource(), "Failed to create GPass buffers");
		}

		void CreatePsoAndRootsig()
		{
			assert_throw(!g_gpassPso && !g_gpassRootsig, "GPass PSO or root signature already created");
			RootParameter params[1] = {};
			params[0].AsConstants(1, D3D12_SHADER_VISIBILITY_ALL, 1);
			RootSignature rootSig(&params[0], (uint32_t)std::size(params));
			g_gpassRootsig = rootSig.Create();
			assert(g_gpassRootsig);
			SET_NAME(g_gpassRootsig, L"GPass Root signature");

			// PSO
			struct
			{
				SubRootSignature rootSig{ g_gpassRootsig };
				SubVertexShader vs{ shaders::GetEngineShader(shaders::ENGINE_SHADER::VS_FULLSCREEN) };
				SubPixelShader ps{ shaders::GetEngineShader(shaders::ENGINE_SHADER::PS_FILLCOLOR) };
				SubPrimitiveTopology topology{ D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE };
				SubRenderTargetFormats rtvFormats;
				SubDepthStenciFormat dsvFormat{ g_depthBufferFormat };
				SubRasterizer rasterizer{ gfx::RASTERIZER_STATE.NO_CULL };
				SubDepthStencil1 depthStencil{ gfx::DEPTH_STATE.DISABLED };
			} stream;
			D3D12_RT_FORMAT_ARRAY rtvFormats = {};
			rtvFormats.NumRenderTargets = 1;
			rtvFormats.RTFormats[0] = g_mainBufferFormat;

			stream.rtvFormats = rtvFormats;

			g_gpassPso = CreatePSO(&stream, sizeof(stream));
			SET_NAME(g_gpassPso, L"GPass PSO");
			assert_throw(g_gpassPso && g_gpassRootsig, "Failed to create GPass Rootsig or PSO");
		}
	}

	

	void Initialize()
	{
		LOG_TRACE("Initializing gpass...");
		CreateBuffers(g_geomBufferSize);
		CreatePsoAndRootsig();
	}

	void Shutdown()
	{
		g_mainBuffer.Release();
		g_depthBuffer.Release();
		g_geomBufferSize = { 100, 100 };

		RELEASE(g_gpassPso);
		RELEASE(g_gpassRootsig);
	}

	void UpdateSize(uint32_t width, uint32_t height)
	{
		if (width > g_geomBufferSize.x || height > g_geomBufferSize.y)
		{
			g_geomBufferSize = {std::max(width, g_geomBufferSize.x), std::max(height, g_geomBufferSize.y) };
			CreateBuffers(g_geomBufferSize);
		}
	}

	void DepthPrepass(ID3D12GraphicsCommandList6* cmd, const GFX_FRAME_DESC& desc)
	{

	}

	void Render(ID3D12GraphicsCommandList6* cmd, const GFX_FRAME_DESC& desc)
	{
		cmd->SetGraphicsRootSignature(g_gpassRootsig);
		cmd->SetPipelineState(g_gpassPso);
		cmd->SetGraphicsRoot32BitConstant(0, 41, 0);
		cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmd->DrawInstanced(3, 1, 0, 0);
	}

	void SetPipelineForDepthPrepass(ID3D12GraphicsCommandList6* cmd)
	{
		const D3D12_CPU_DESCRIPTOR_HANDLE dsv = g_depthBuffer.DSVAllocation();
		cmd->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 0.0f, 0, 0, nullptr);
		cmd->OMSetRenderTargets(0, nullptr, FALSE, &dsv);
	}

	void SetPipelineForGPass(ID3D12GraphicsCommandList6* cmd)
	{
		const D3D12_CPU_DESCRIPTOR_HANDLE rtv = g_mainBuffer.RTVAllocation(0);
		const D3D12_CPU_DESCRIPTOR_HANDLE dsv = g_depthBuffer.DSVAllocation();
		cmd->ClearRenderTargetView(rtv, CLEAR_COLOR, 0, nullptr);
		cmd->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
	}

	const RenderTexture& GetMainBuffer() { return g_mainBuffer; }
	const DepthTexture& GetDepthBuffer() { return g_depthBuffer; }

#ifdef _DEBUG_GRAPHICS
	void DrawDebugInfo(uint16_t width, uint16_t height)
	{
		if (ImGui::CollapsingHeader("Graphics pass", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Geometry buffers size: %dx%d", g_geomBufferSize.x, g_geomBufferSize.y);

			/*if (ImPlot::BeginPlot("FPS", NULL, NULL, ImVec2(-1, 100), ImPlotFlags_NoMenus | ImPlotFlags_NoLegend))
			{
				ImPlot::PlotLine<float>("FPS", g_dbgGpassfpsHistory, 500, 1.0, 0, g_dbgGpassfpsIndex, sizeof(float));
				ImPlot::EndPlot();
			}*/

			ImGui::Columns(2);
			float aspectRatio = (float)width / (float)height;
			float availableWidth = ImGui::GetContentRegionAvail().x;
			float previewHeight = availableWidth / aspectRatio;
			ImVec2 uv0(0, 0);
			ImVec2 uv1((float)width / g_geomBufferSize.x, (float)height / g_geomBufferSize.y);

			ImGui::Text("Main buffer");
			ImGui::Image((ImTextureID)g_mainBuffer.SRVAllocation().GPU.ptr, ImVec2(availableWidth, previewHeight), uv0, uv1, ImVec4(1, 1, 1, 1), ImVec4(4, 4, 4, 4));
			//ImGui::SameLine();
			ImGui::Text("Depth map");
			ImGui::Image((ImTextureID)g_depthBuffer.SRVAllocation().GPU.ptr, ImVec2(availableWidth, previewHeight), uv0, uv1, ImVec4(1, 1, 1, 1), ImVec4(4, 4, 4, 4));

			ImGui::NextColumn();

			ImGui::Text("Normal map");
			ImGui::Image((ImTextureID)g_depthBuffer.SRVAllocation().GPU.ptr, ImVec2(availableWidth, previewHeight), uv0, uv1, ImVec4(1, 1, 1, 1), ImVec4(4, 4, 4, 4));
			//ImGui::SameLine();
			ImGui::Text("[RESERVER]");
			ImGui::Image((ImTextureID)g_mainBuffer.SRVAllocation().GPU.ptr, ImVec2(availableWidth, previewHeight), uv0, uv1, ImVec4(1, 1, 1, 1), ImVec4(4, 4, 4, 4));
			ImGui::Columns(1);
		}
	}
#endif
}