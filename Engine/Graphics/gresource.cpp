#include "gresource.h"

namespace engine::gfx
{
#pragma region Texture

	Texture::Texture(Texture&& o)
		: m_res(o.m_res), m_srv(o.m_srv)
	{
		assert(this != &o);
		o.m_res = nullptr;
		o.m_srv = {};
	}

	Texture& Texture::operator=(Texture&& o)
	{
		assert(this != &o);
		Release();
		m_res = o.m_res;
		m_srv = o.m_srv;
		o.m_res = nullptr;
		o.m_srv = {};
		return *this;
	}

	Texture::Texture(D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, ID3D12Resource* resource)
		: m_res(resource)
	{
		assert(device);
		m_srv = g_srvHeap.Allocate();
		device->CreateShaderResourceView(m_res, srvDesc, m_srv.CPU);
	}

	Texture::Texture(TEXTURE_DESC& desc, D3D12_RESOURCE_ALLOCATION_INFO1& info, ID3D12Heap* heap)
	{
		assert(device);
		const D3D12_CLEAR_VALUE* const clearVal = (desc.resDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET || desc.resDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) ? &desc.clearValue : NULL;
		device->CreatePlacedResource(heap, info.Offset, &desc.resDesc, desc.initialState, clearVal, IID_PPV_ARGS(&m_res));
		m_srv = g_srvHeap.Allocate();
		device->CreateShaderResourceView(m_res, &desc.srvDesc, m_srv.CPU);
	}

	Texture::Texture(TEXTURE_DESC& desc)
	{
		assert(device);
		const D3D12_CLEAR_VALUE* const clearVal = (desc.resDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET || desc.resDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) ? &desc.clearValue : NULL;
		D3D12_HEAP_PROPERTIES heapProps;
		ZeroMemory(&heapProps, sizeof(D3D12_HEAP_PROPERTIES));
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc.resDesc, desc.initialState, clearVal, IID_PPV_ARGS(&m_res));
		m_srv = g_srvHeap.Allocate();
		device->CreateShaderResourceView(m_res, &desc.srvDesc, m_srv.CPU);
	}

	void Texture::Release()
	{
		g_srvHeap.Free(m_srv);
		RELEASE(m_res);
	}
#pragma endregion
#pragma region RenderTex

	RenderTexture::RenderTexture(TEXTURE_DESC& desc)
		: m_tex(desc)
	{
		m_mipCount = desc.resDesc.MipLevels;
		assert(m_mipCount <= MAX_MIPS);
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = desc.resDesc.Format;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		for (uint32_t i = 0; i < m_mipCount; ++i)
		{
			m_rtv[i] = g_rtvHeap.Allocate();
			device->CreateRenderTargetView(m_tex.Resource(), &rtvDesc, m_rtv[i].CPU);
			rtvDesc.Texture2D.MipSlice++;
		}
	}

	RenderTexture::RenderTexture(RenderTexture&& o)
		: m_tex(std::move(o.m_tex)), m_mipCount(o.m_mipCount)
	{
		assert(this != &o);
		for (uint32_t i = 0; i < MAX_MIPS; ++i)
			m_rtv[i] = o.m_rtv[i];
		o.m_mipCount = 0;
		for (uint32_t i = 0; i < MAX_MIPS; ++i)
			o.m_rtv[i] = {};
	}

	RenderTexture& RenderTexture::operator=(RenderTexture&& o)
	{
		assert(this != &o);
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
			g_rtvHeap.Free(m_rtv[i]);
		m_mipCount = 0;
	}
#pragma endregion

#pragma region DepthTex

	DepthTexture::DepthTexture(TEXTURE_DESC& desc)
	{
		DXGI_FORMAT format = desc.resDesc.Format;
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		if (desc.resDesc.Format == DXGI_FORMAT_D32_FLOAT)
		{
			desc.resDesc.Format == DXGI_FORMAT_R32_TYPELESS;
			srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}

		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.PlaneSlice = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		desc.srvDesc = srvDesc;
		m_tex = Texture(desc);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = format;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.Texture2D.MipSlice = 0;

		m_dsv = g_dsvHeap.Allocate();
		device->CreateDepthStencilView(m_tex.Resource(), &dsvDesc, m_dsv.CPU);
	}

	DepthTexture::DepthTexture(DepthTexture&& o)
		: m_tex(std::move(o.m_tex)), m_dsv(o.m_dsv)
	{
		assert(this != &o);
		o.m_dsv = {};
	}

	DepthTexture& DepthTexture::operator=(DepthTexture&& o)
	{
		assert(this != &o);
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