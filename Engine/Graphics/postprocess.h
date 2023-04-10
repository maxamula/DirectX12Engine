#pragma once
#include "graphics.h"

namespace engine::gfx::fx
{
	bool Initialize();
	void Shutdown();
	void PostProcess(ID3D12GraphicsCommandList6* cmd, D3D12_CPU_DESCRIPTOR_HANDLE rtv);
#ifdef _DEBUG_GRAPHICS
	void DrawDebugInfo(uint16_t width, uint16_t height);
#endif
}