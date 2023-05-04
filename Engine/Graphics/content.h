#pragma once
#include "graphics.h"

namespace engine::gfx
{
	uint64_t AddSubmesh(const uint8_t*& data);
	void RemoveSubmesh(uint64_t id);
}
