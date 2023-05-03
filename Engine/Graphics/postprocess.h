#pragma once
#include "graphics.h"

namespace engine::gfx::fx
{
	void Initialize();
	void Shutdown();
	void PostProcess(ID3D12GraphicsCommandList6* cmd);
#ifdef _DEBUG_GRAPHICS
	void DrawDebugInfo(uint16_t width, uint16_t height);
#endif
}