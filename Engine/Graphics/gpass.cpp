#include "gpass.h"
#include "gresource.h"

#if _DEBUG
constexpr float CLEAR_COLOR[4]{ 0.5f, 0.5f, 0.5f, 1.f };
#else
constexpr float CLEAR_COLOR[4]{ };
#endif

namespace engine::gfx::gpass
{
	namespace
	{
		RenderTexture g_mainBuffer;
		DepthTexture g_depthBuffer;
		const DXGI_FORMAT g_mainBufferFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
		const DXGI_FORMAT g_depthBufferFormat = DXGI_FORMAT_D32_FLOAT;

		bool CreateBuffers(uint32_t width, uint32_t height)
		{
			g_mainBuffer.Release();
			g_depthBuffer.Release();

			D3D12_RESOURCE_DESC desc = {};
			desc.Alignment = 0;
			desc.DepthOrArraySize = 1;
			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			desc.Format = g_mainBufferFormat;
			desc.Height = height;
			desc.Width = width;
			desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			desc.MipLevels = 0;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;

			// Create main texture
			{
				TEXTURE_DESC texDesc = {};
				texDesc.resDesc = desc;
				texDesc.initialState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
				texDesc.clearValue.Format = desc.Format;

				// Shader resource view desc
				texDesc.srvDesc.Format = desc.Format;
				texDesc.srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				texDesc.srvDesc.Texture2D.MipLevels = 1;
				texDesc.srvDesc.Texture2D.MostDetailedMip = 0;
				texDesc.srvDesc.Texture2D.PlaneSlice = 0;
				texDesc.srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				texDesc.srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				memcpy(&texDesc.clearValue.Color, &CLEAR_COLOR[0], sizeof(float) * std::size(CLEAR_COLOR));
				g_mainBuffer = RenderTexture(texDesc);
			}

			desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			desc.Format = g_depthBufferFormat;
			desc.MipLevels = 1;

			//Create depth texture
			{
				TEXTURE_DESC texDesc = {};
				texDesc.resDesc = desc;
				texDesc.initialState = D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
				texDesc.clearValue.Format = desc.Format;
				texDesc.clearValue.DepthStencil.Depth = 0.0f;
				texDesc.clearValue.DepthStencil.Stencil = 0;
				g_depthBuffer = DepthTexture(texDesc);
			}
			return g_mainBuffer.Resource() && g_depthBuffer.Resource();
		}
	}



	void Initialize()
	{
		assert(CreateBuffers(100, 100));
	}

	void Shutdown()
	{
		g_mainBuffer.Release();
		g_depthBuffer.Release();
	}
}