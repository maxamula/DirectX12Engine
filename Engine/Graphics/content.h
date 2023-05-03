#pragma once
#include "graphics.h"

namespace engine::gfx
{
	namespace submesh
	{
		uint32_t Add(const uint8_t*& data);
		void Remove(uint32_t id);
	}
}
