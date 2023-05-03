#pragma once
#include "graphics.h"

namespace engine::gfx
{
	ID3D12Resource* CreateBuffer(uint32_t bufferSize, const void* data = nullptr,
		bool cpuVisible = false, D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE, ID3D12Heap* heap = nullptr, uint64_t offset = 0);
}
