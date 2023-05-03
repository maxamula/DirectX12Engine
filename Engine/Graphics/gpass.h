#pragma once
#include "graphics.h"
#include "gresource.h"
#include "rootsig.h"
#include "pso.h"
#include "shaders.h"

namespace engine::gfx
{
	struct GFX_FRAME_DESC;
	namespace gpass
	{
		void Initialize();
		void Shutdown();
		void UpdateSize(uint32_t width, uint32_t height);
		void SetPipelineForDepthPrepass(ID3D12GraphicsCommandList6* cmd);
		void SetPipelineForGPass(ID3D12GraphicsCommandList6* cmd);
		void DepthPrepass(ID3D12GraphicsCommandList6* cmd, const gfx::GFX_FRAME_DESC& desc);
		void Render(ID3D12GraphicsCommandList6* cmd, const gfx::GFX_FRAME_DESC& desc);

		// Accessors
		[[nodiscard]] const RenderTexture& GetMainBuffer();
		[[nodiscard]] const DepthTexture& GetDepthBuffer();

		// Misc
#ifdef _DEBUG_GRAPHICS
		void DrawDebugInfo(uint16_t width, uint16_t height);
#endif
	}
}

