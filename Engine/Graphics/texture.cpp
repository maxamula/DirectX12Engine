#include "texture.h"

namespace engine::gfx
{
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
		const D3D12_CLEAR_VALUE* const clearVal = (desc.resDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET || desc.resDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) ? desc.clearValue : NULL;
		device->CreatePlacedResource(heap, info.Offset, &desc.resDesc, desc.initialState, clearVal, IID_PPV_ARGS(&m_res));
		m_srv = g_srvHeap.Allocate();
		device->CreateShaderResourceView(m_res, &desc.srvDesc, m_srv.CPU);
	}

	Texture::Texture(TEXTURE_DESC& desc)
	{
		assert(device);
		const D3D12_CLEAR_VALUE* const clearVal = (desc.resDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET || desc.resDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) ? desc.clearValue : NULL;
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
}