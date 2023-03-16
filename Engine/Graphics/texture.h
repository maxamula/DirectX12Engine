#pragma once
#include "graphics.h"

#define MAX_MIPS 10

namespace engine::gfx
{
	struct TEXTURE_DESC
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		D3D12_RESOURCE_DESC resDesc;
		D3D12_CLEAR_VALUE* clearValue;
		D3D12_RESOURCE_STATES initialState{};
	};

	class Texture
	{
	public:
		Texture() = default;
		DISABLE_COPY(Texture);
		// Move
		Texture(Texture&& o);
		Texture& operator=(Texture&& o);

		// From existing resource
		Texture(D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, ID3D12Resource* resource);
		// Place resource
		Texture(TEXTURE_DESC& desc, D3D12_RESOURCE_ALLOCATION_INFO1& info, ID3D12Heap* heap);
		// Create texture
		Texture(TEXTURE_DESC& desc);
		
		// DESTRUCTOR
		~Texture() { Release(); }

		inline ID3D12Resource* Resource() const { return m_res; }
		inline DESCRIPTOR_HANDLE SRVAllocation() const { return m_srv; }
		virtual void Release();

	protected:
		static constexpr uint16_t maxMips{ 10 };
		ID3D12Resource* m_res;
		DESCRIPTOR_HANDLE m_srv;
	};

	class RenderTexture
	{
	public:
		RenderTexture() = default;
		RenderTexture(TEXTURE_DESC& desc);
		DISABLE_COPY(RenderTexture);
		// Move
		RenderTexture(RenderTexture&& o);
		RenderTexture& operator=(RenderTexture&& o);

		void Release();
	private:
		Texture m_tex;
		DESCRIPTOR_HANDLE m_rtv[MAX_MIPS]{};
		uint32_t m_mipCount = 0;
	};
}
