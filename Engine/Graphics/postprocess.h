#pragma once
#include "graphics.h"

namespace engine::gfx::fx
{
	bool Initialize();
	void Shutdown();
	void PostProcess(ID3D12GraphicsCommandList6* cmd, D3D12_CPU_DESCRIPTOR_HANDLE rtv);
}