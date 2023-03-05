#include "depthtexture.h"

namespace engine::gfx
{
	DepthTexture::DepthTexture(TEXTURE_DESC& desc)
	{
		assert(device);
		const DXGI_FORMAT dsvFormat = desc.resDesc.Format;
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		if (desc.resDesc.Format == DXGI_FORMAT_D32_FLOAT)
		{
			desc.resDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.PlaneSlice = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		desc.srvDesc = srvDesc;
		Texture::Texture(desc);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.Format = dsvFormat;
		dsvDesc.Texture2D.MipSlice = 0;

		m_dsv = g_dsvHeap.Allocate();
		device->CreateDepthStencilView(m_res, &dsvDesc, m_dsv.CPU);
	}

	void DepthTexture::Release()
	{
		g_dsvHeap.Free(m_dsv);
		Texture::Release();
	}
}