#pragma once
#include "graphics.h"
#include "descriptorheap.h"
#include "texture.h"

namespace engine::gfx
{
	class RenderTexture : public Texture
	{
	public:
		RenderTexture() = default;

		// DESTRUCTOR
		~RenderTexture() { Release(); }

		void Release() override;
	private:
		void _Initialize();
		DESCRIPTOR_HANDLE m_rtv[Texture::maxMips]{};
		uint32_t m_mipCount = 0;

	};
}
