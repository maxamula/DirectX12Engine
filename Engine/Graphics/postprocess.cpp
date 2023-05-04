#include "postprocess.h"
#include "gresource.h"
#include "rootsig.h"
#include "pso.h"
#include "shaders.h"
#include "gpass.h"
#include "overlay.h"

namespace engine::gfx::fx
{
	namespace
	{
		float g_clearColor[4]{ 0.1f, 0.0f, 0.0f, 0.0f };
		ID3D12RootSignature* g_fxRootSig = nullptr;
		ID3D12PipelineState* g_fxPso = nullptr;

		struct FX_ROOT_PARAM
		{
			enum : uint32_t
			{
				ROOT_CONSTANSTS,
				NUM_ROOT_PARAMS
			};
		};

		bool FxCreatePSOAndRootsig()
		{
			assert_throw(!g_fxRootSig && !g_fxPso, "FX PSO or rootsig already created");

			DescriptorRange range { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE };

			RootParameter params[FX_ROOT_PARAM::NUM_ROOT_PARAMS] = {};
			params[FX_ROOT_PARAM::ROOT_CONSTANSTS].AsConstants(1, D3D12_SHADER_VISIBILITY_ALL, 1);
			RootSignature rootSig(&params[0], (uint32_t)std::size(params));
			g_fxRootSig = rootSig.Create();
			assert(g_fxRootSig);
			SET_NAME(g_fxRootSig, L"FX Root signature");

			// PSO
			struct
			{
				SubRootSignature rootSig{ g_fxRootSig };
				SubVertexShader vs{ shaders::GetEngineShader(shaders::ENGINE_SHADER::VS_FULLSCREEN) };
				SubPixelShader ps{ shaders::GetEngineShader(shaders::ENGINE_SHADER::PS_POSTPROCESS) };
				SubPrimitiveTopology topology{ D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE };
				SubRenderTargetFormats rtvFormats;
				SubRasterizer rasterizer{ gfx::RASTERIZER_STATE.NO_CULL };
			} stream;
			D3D12_RT_FORMAT_ARRAY rtvFormats = {};
			rtvFormats.NumRenderTargets = 1;
			rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

			stream.rtvFormats = rtvFormats;

			g_fxPso = CreatePSO(&stream, sizeof(stream));
			SET_NAME(g_fxPso, L"FX PSO");
		}
	}

	void Initialize()
	{
		LOG_TRACE("Initializing fx...");
		FxCreatePSOAndRootsig();
	}

	void Shutdown()
	{
		RELEASE(g_fxRootSig);
		RELEASE(g_fxPso);
	}

	void PostProcess(ID3D12GraphicsCommandList6* cmd)
	{
		// TODO
		cmd->SetGraphicsRootSignature(g_fxRootSig);
		cmd->SetPipelineState(g_fxPso);
		cmd->SetGraphicsRoot32BitConstant(FX_ROOT_PARAM::ROOT_CONSTANSTS, gpass::GetMainBuffer().SRVAllocation().GetIndex(), 0);
		cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmd->DrawInstanced(3, 1, 0, 0);
	}

#ifdef _DEBUG_GRAPHICS
	void DrawDebugInfo(uint16_t width, uint16_t height)
	{
		if (ImGui::CollapsingHeader("Post-processing", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::ColorPicker3("RTV clear color", g_clearColor, ImGuiColorEditFlags_NoOptions);
		}
	}
#endif
}