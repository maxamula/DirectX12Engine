#pragma once
#include "graphics.h"
#include "texture.h"

namespace engine::gfx
{
	class DepthTexture : public Texture
	{
	public:
		DepthTexture() = default;
		DepthTexture(TEXTURE_DESC& desc);
		DISABLE_MOVE_COPY(DepthTexture);
		// DESTRUCTOR
		~DepthTexture() { Release(); }

		void Release() override;
	private:
		DESCRIPTOR_HANDLE m_dsv;
	};
}