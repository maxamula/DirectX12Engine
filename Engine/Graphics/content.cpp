#include "content.h"
#include "utils.h"
#include "buffers.h"
#include <ResourceUploadBatch.h>

namespace engine::gfx
{
	namespace
	{
		template<uint64_t alignment>
		constexpr uint64_t AlignUp(uint64_t value)
		{
			return (value + (alignment - 1)) & ~(alignment - 1);
		}

		template<uint64_t alignment>
		constexpr uint64_t AlignDown(uint64_t value)
		{
			return value & ~(alignment - 1);
		}

		struct SUBMESH
		{
			ID3D12Resource* buffer;
			struct
			{
				D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
				D3D12_INDEX_BUFFER_VIEW indexBufferView{};
			} meshView;
			struct
			{
				uint32_t componentsMask{};
				D3D_PRIMITIVE_TOPOLOGY topology{};
			} elementsView;
		};

		utils::ItemManager<SUBMESH> g_submeshes; // not thread safe
		std::mutex g_submeshesMutex;
	}

	namespace submesh
	{
		uint32_t Add(const uint8_t*& data)
		{
			utils::BinaryReader br((void*)data);
			const uint32_t componentsMask = br.ReadUInt32();
			const uint32_t vertexSize = br.ReadUInt32();
			const uint32_t vertexCount = br.ReadUInt32();
			const uint32_t indexSize = br.ReadUInt32();
			const uint32_t indexCount = br.ReadUInt32();
			const uint32_t vertexDataSize = vertexSize * vertexCount;
			const uint32_t indexDataSize = indexSize * indexCount;

			constexpr uint32_t alignment = D3D12_STANDARD_MAXIMUM_ELEMENT_ALIGNMENT_BYTE_MULTIPLE;
			const uint32_t vertexDataSizeAligned = AlignUp<alignment>(vertexDataSize);
			const uint32_t indexDataSizeAligned = AlignUp<alignment>(indexDataSize);
			const uint32_t bufferSize = vertexDataSizeAligned + indexDataSizeAligned;

			
			D3D12_RESOURCE_DESC desc{};
			desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			desc.Alignment = 0;
			desc.Width = bufferSize;
			desc.Height = 1;
			desc.DepthOrArraySize = 1;
			desc.MipLevels = 1;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			desc.Flags = D3D12_RESOURCE_FLAG_NONE;

			ID3D12Resource* combinedBuffer = nullptr;

			ThrowIfFailed(device->CreateCommittedResource(&HEAP.UPLOAD, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&combinedBuffer)));
			DirectX::ResourceUploadBatch uploadBatch(device);
			uploadBatch.Begin();
			D3D12_SUBRESOURCE_DATA subresourceData{};
			subresourceData.pData = br.CurrentReadPos();
			subresourceData.RowPitch = vertexDataSizeAligned;
			subresourceData.SlicePitch = vertexDataSizeAligned;
			uploadBatch.Upload(combinedBuffer, 0, &subresourceData, 1);
			br.Skip(vertexDataSize);
			
			subresourceData.pData = br.CurrentReadPos();
			subresourceData.RowPitch = indexDataSizeAligned;
			subresourceData.SlicePitch = indexDataSizeAligned;
			uploadBatch.Upload(combinedBuffer, vertexDataSizeAligned, &subresourceData, 1);
			uploadBatch.Transition(combinedBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			uploadBatch.End(g_cmdQueue.GetCommandQueue());

			SUBMESH submesh{};
			submesh.meshView.vertexBufferView.BufferLocation = combinedBuffer->GetGPUVirtualAddress();
			submesh.meshView.vertexBufferView.SizeInBytes = vertexDataSize;
			submesh.meshView.vertexBufferView.StrideInBytes = vertexSize;

			submesh.meshView.indexBufferView.BufferLocation = combinedBuffer->GetGPUVirtualAddress() + vertexDataSizeAligned;
			submesh.meshView.indexBufferView.SizeInBytes = indexDataSize;
			submesh.meshView.indexBufferView.Format = indexSize == sizeof(uint16_t) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

			submesh.elementsView.componentsMask = componentsMask;
			submesh.elementsView.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; // Read from file

			submesh.buffer = combinedBuffer;

			std::lock_guard lock(g_submeshesMutex);
			return g_submeshes.Add(submesh);
		}

		void Remove(uint32_t id)
		{
			// if object is still in use, it will be removed later
			g_cmdQueue.DeferedReleaseEvent.FireOnce([&]()
			{
				std::lock_guard lock(g_submeshesMutex);
				g_submeshes.Get(id)->buffer->Release();
				g_submeshes.Remove(id);
			});
		}
	}
}