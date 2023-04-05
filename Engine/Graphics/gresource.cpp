#include "gresource.h"

namespace engine::gfx
{
	ResourceBarrier g_resourceBarriers{};

	void TransitionResource(ID3D12GraphicsCommandList6* cmd, ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after
		, D3D12_RESOURCE_BARRIER_FLAGS flags, uint32_t subresource)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = flags;
		barrier.Transition.pResource = resource;
		barrier.Transition.Subresource = subresource;
		barrier.Transition.StateBefore = before;
		barrier.Transition.StateAfter = after;
		cmd->ResourceBarrier(1, &barrier);
	}

	void ResourceBarrier::Add(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after
		, D3D12_RESOURCE_BARRIER_FLAGS flags, uint32_t subresource)
	{
		assert(m_offset < MAX_RES_BARRIERS);
		assert(resource);
		D3D12_RESOURCE_BARRIER& barrier = m_barriers[m_offset++];
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = flags;
		barrier.Transition.pResource = resource;
		barrier.Transition.Subresource = subresource;
		barrier.Transition.StateBefore = before;
		barrier.Transition.StateAfter = after;
	}
	void ResourceBarrier::Add(ID3D12Resource* resource, D3D12_RESOURCE_BARRIER_FLAGS flags)
	{
		assert(m_offset < MAX_RES_BARRIERS);
		assert(resource);
		D3D12_RESOURCE_BARRIER& barrier = m_barriers[m_offset++];
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
		barrier.Flags = flags;
		barrier.UAV.pResource = resource;
	}

	void ResourceBarrier::Add(ID3D12Resource* resBefore, ID3D12Resource* resAfter, D3D12_RESOURCE_BARRIER_FLAGS flags)
	{
		assert(m_offset < MAX_RES_BARRIERS);
		D3D12_RESOURCE_BARRIER& barrier = m_barriers[m_offset++];
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
		barrier.Flags = flags;
		barrier.Aliasing.pResourceBefore = resBefore;
		barrier.Aliasing.pResourceAfter = resAfter;
	}

	void ResourceBarrier::Flush(ID3D12GraphicsCommandList6* cmd)
	{
		if (m_offset > 0)
		{
			cmd->ResourceBarrier(m_offset, m_barriers);
			m_offset = 0;
		}
	}

#pragma region Texture

	Texture::Texture(Texture&& o)
		: m_res(o.m_res), m_srv(o.m_srv)
	{
		assert_throw(this != &o, "Self assignment");
		o.m_res = nullptr;
		o.m_srv = {};
	}

	Texture& Texture::operator=(Texture&& o)
	{
		assert_throw(this != &o, "Self assignment");
		Release();
		m_res = o.m_res;
		m_srv = o.m_srv;
		o.m_res = nullptr;
		o.m_srv = {};
		return *this;
	}

	Texture::Texture(TEXTURE_DESC& desc)
	{
		DEVICE_CHECK;
		D3D12_CLEAR_VALUE* clearVal = (desc.pResDesc->Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET || desc.pResDesc->Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) ? &desc.clearValue : nullptr;

		if (desc.resource)
		{
			m_res = desc.resource;
		}
		else if (desc.pHeap)
		{
			succeed(device->CreatePlacedResource(desc.pHeap, desc.allocInfo.Offset, desc.pResDesc, desc.initialState, clearVal, IID_PPV_ARGS(&m_res)), "Failed to create placed resource");
		}
		else
		{
			D3D12_HEAP_PROPERTIES defaultHeap
			{
				D3D12_HEAP_TYPE_DEFAULT,
				D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
				D3D12_MEMORY_POOL_UNKNOWN,
				0,
				0
			};
			succeed(device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, desc.pResDesc, desc.initialState, clearVal, IID_PPV_ARGS(&m_res)), "Failed to create committed resource");
		}
		
		assert(m_res);
		m_srv = g_srvHeap.Allocate();
		device->CreateShaderResourceView(m_res, desc.pSrvDesc, m_srv.CPU);
	}

	void Texture::Release()
	{
		g_srvHeap.Free(m_srv);
		m_srv = {};
		RELEASE(m_res);
	}
#pragma endregion
#pragma region RenderTex

	RenderTexture::RenderTexture(TEXTURE_DESC& desc)
		: m_tex{desc}
	{
		m_mipCount = Resource()->GetDesc().MipLevels;
		assert_throw(m_mipCount && m_mipCount < MAX_MIPS, "Failed to create texture. Invalid mip count.");

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = desc.pResDesc->Format;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		for (uint32_t i = 0; i < m_mipCount; i++)
		{
			m_rtv[i] = g_rtvHeap.Allocate();
			device->CreateRenderTargetView(Resource(), &rtvDesc, m_rtv[i].CPU);
			++rtvDesc.Texture2D.MipSlice;
		}
	}

	RenderTexture::RenderTexture(RenderTexture&& o)
		: m_tex(std::move(o.m_tex)), m_mipCount(o.m_mipCount)
	{
		assert_throw(this != &o, "Self assignment");
		for (uint32_t i = 0; i < MAX_MIPS; ++i)
			m_rtv[i] = o.m_rtv[i];
		o.m_mipCount = 0;
		for (uint32_t i = 0; i < MAX_MIPS; ++i)
			o.m_rtv[i] = {};
	}

	RenderTexture& RenderTexture::operator=(RenderTexture&& o)
	{
		assert_throw(this != &o, "Self assignment");
		m_tex.Release();
		m_tex = std::move(o.m_tex);
		m_mipCount = o.m_mipCount;
		for (uint32_t i = 0; i < MAX_MIPS; ++i)
			m_rtv[i] = o.m_rtv[i];
		o.m_mipCount = 0;
		for (uint32_t i = 0; i < MAX_MIPS; ++i)
			o.m_rtv[i] = {};
		return *this;
	}

	void RenderTexture::Release()
	{
		m_tex.Release();
		for (uint32_t i = 0; i < MAX_MIPS; i++)
		{
			g_rtvHeap.Free(m_rtv[i]);
			m_rtv[i] = {};
		}
		m_mipCount = 0;
	}

	uint32_t RenderTexture::_CalculateMipLevels(uint32_t width, uint32_t height)
	{
		uint32_t mipLevels = 1;
		while (width > 1 || height > 1)
		{
			width = std::max(1u, width / 2);
			height = std::max(1u, height / 2);
			mipLevels++;
		}
		return mipLevels;
	}
#pragma endregion

#pragma region DepthTex

	DepthTexture::DepthTexture(TEXTURE_DESC& desc)
	{
		const DXGI_FORMAT dsvFormat = desc.pResDesc->Format;
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		if (desc.pResDesc->Format == DXGI_FORMAT_D32_FLOAT)
		{
			desc.pResDesc->Format == DXGI_FORMAT_R32_TYPELESS;
			srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}

		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.PlaneSlice = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		assert_throw(!desc.pSrvDesc, "Depth texture cannot have a custom SRV description");
		desc.pSrvDesc = &srvDesc;
		m_tex = Texture(desc);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = dsvFormat;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.Texture2D.MipSlice = 0;

		m_dsv = g_dsvHeap.Allocate();
		device->CreateDepthStencilView(m_tex.Resource(), &dsvDesc, m_dsv.CPU);
	}

	DepthTexture::DepthTexture(DepthTexture&& o)
		: m_tex(std::move(o.m_tex)), m_dsv(o.m_dsv)
	{
		assert_throw(this != &o, "Self assignment");
		o.m_dsv = {};
	}

	DepthTexture& DepthTexture::operator=(DepthTexture&& o)
	{
		assert_throw(this != &o, "Self assignment");
		m_tex.Release();
		m_tex = std::move(o.m_tex);
		m_dsv = o.m_dsv;
		o.m_dsv = {};
		return *this;
	}

	void DepthTexture::Release()
	{
		m_tex.Release();
		g_dsvHeap.Free(m_dsv);
		m_dsv = {};
	}

#pragma endregion
}