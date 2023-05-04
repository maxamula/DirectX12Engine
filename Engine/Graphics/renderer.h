#pragma once
#include "graphics.h"

namespace engine::gfx
{
	struct GFX_FRAME_RENDER_TARGET_DESC
	{
		uint32_t rtvCount = 1;
		RENDER_TARGET* renderTargets = nullptr;
		D3D12_VIEWPORT* viewports = nullptr;
		D3D12_RECT* scissiors = nullptr;
		uint32_t surfWidth;
		uint32_t surfHeight;
	};

	ENGINE_API void RenderFrame(GFX_FRAME_RENDER_TARGET_DESC& desc);
	void RenderHud(GFX_FRAME_RENDER_TARGET_DESC& desc);
}
