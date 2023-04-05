#pragma once
#include "graphics.h"

#define MAX_MIPS 17

namespace engine::gfx
{
	class ResourceBarrier;
	extern ResourceBarrier g_resourceBarriers;

	struct TEXTURE_DESC
	{
		ID3D12Resource* resource = nullptr;

		ID3D12Heap1* pHeap = nullptr;
		D3D12_RESOURCE_ALLOCATION_INFO1 allocInfo = {};

		D3D12_SHADER_RESOURCE_VIEW_DESC* pSrvDesc = nullptr;
		D3D12_RESOURCE_DESC* pResDesc;
		D3D12_RESOURCE_STATES initialState = {};
		D3D12_CLEAR_VALUE clearValue;
	};

	void TransitionResource(ID3D12GraphicsCommandList6* cmd, ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after
		, D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, uint32_t subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

	class ResourceBarrier
	{
	public:
		const static uint32_t MAX_RES_BARRIERS{ 32 };
		void Add(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after
			, D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, uint32_t subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
		void Add(ID3D12Resource* resource, D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE);
		void Add(ID3D12Resource* resBefore, ID3D12Resource* resAfter, D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE);
		void Flush(ID3D12GraphicsCommandList6* cmd);
	private:
		D3D12_RESOURCE_BARRIER m_barriers[MAX_RES_BARRIERS]{};
		uint32_t m_offset = 0;
	};

	class Texture
	{
	public:
		Texture() = default;
		DISABLE_COPY(Texture);
		// Move
		Texture(Texture&& o);
		Texture& operator=(Texture&& o);

		Texture(TEXTURE_DESC& desc);
		// DESTRUCTOR
		~Texture() { assert(!m_res); }

		inline void Update(D3D12_SHADER_RESOURCE_VIEW_DESC* pSrvDesc = nullptr) { device->CreateShaderResourceView(m_res, pSrvDesc, m_srv.CPU); }
		inline ID3D12Resource* Resource() const { return m_res; }
		inline DESCRIPTOR_HANDLE SRVAllocation() const { return m_srv; }
		virtual void Release();

	protected:
		static constexpr uint16_t maxMips{ 10 };
		ID3D12Resource* m_res = nullptr;
		DESCRIPTOR_HANDLE m_srv = {};
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
		inline ID3D12Resource* Resource() const { return m_tex.Resource(); }
		inline void Update(D3D12_SHADER_RESOURCE_VIEW_DESC* pSrvDesc = nullptr) { m_tex.Update(pSrvDesc); }
		inline DESCRIPTOR_HANDLE SRVAllocation() const { return m_tex.SRVAllocation(); }
		inline D3D12_CPU_DESCRIPTOR_HANDLE RTVAllocation(uint32_t mip) const { assert(mip <= m_mipCount); return m_rtv[mip].CPU; }
		inline uint32_t MipCount() const { return m_mipCount; }
	private:
		uint32_t _CalculateMipLevels(uint32_t width, uint32_t height);
		Texture m_tex;
		DESCRIPTOR_HANDLE m_rtv[MAX_MIPS]{};
		uint32_t m_mipCount = 0;
	};

	class DepthTexture
	{
	public:
		DepthTexture() = default;
		DepthTexture(TEXTURE_DESC& desc);
		DISABLE_COPY(DepthTexture);
		// Move
		DepthTexture(DepthTexture&& o);
		DepthTexture& operator=(DepthTexture&& o);

		void Release();
		inline ID3D12Resource* Resource() const { return m_tex.Resource(); }
		inline DESCRIPTOR_HANDLE SRVAllocation() const { return m_tex.SRVAllocation(); }
		inline D3D12_CPU_DESCRIPTOR_HANDLE DSVAllocation() const { return m_dsv.CPU; }
	private:
		Texture m_tex;
		DESCRIPTOR_HANDLE m_dsv = {};
	};
}