#include "buffers.h"
#include "uploadegine.h"
#include <ResourceUploadBatch.h>

namespace engine::gfx
{
	ID3D12Resource* CreateBuffer(uint32_t bufferSize, const void* data,
		bool cpuVisible, D3D12_RESOURCE_STATES state,
		D3D12_RESOURCE_FLAGS flags, ID3D12Heap* heap, uint64_t offset)
	{
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
		desc.Flags = cpuVisible ? D3D12_RESOURCE_FLAG_NONE : flags;
		
		ID3D12Resource* resource = nullptr;
		const D3D12_RESOURCE_STATES initState = cpuVisible ? D3D12_RESOURCE_STATE_GENERIC_READ : state;

		if (heap)
		{
			ThrowIfFailed(device->CreatePlacedResource(heap, offset, &desc, initState, nullptr, IID_PPV_ARGS(&resource)));
		}
		else
		{
			ThrowIfFailed(device->CreateCommittedResource(cpuVisible ? &HEAP.UPLOAD : &HEAP.DEFAULT, D3D12_HEAP_FLAG_NONE, &desc, initState, nullptr, IID_PPV_ARGS(&resource)));
		}	

		if (data)
		{
			if (cpuVisible)
			{
				D3D12_RANGE range{};
				void* mapped = nullptr;
				ThrowIfFailed(resource->Map(0, &range, &mapped));
				memcpy(mapped, data, bufferSize);
				resource->Unmap(0, nullptr);
			}
			else
			{
				copy::CopyContext copyContext(bufferSize);
				memcpy(copyContext.Mapped(), data, bufferSize);
				copyContext.GetCommandList()->CopyResource(resource, copyContext.GetUploadBuffer());
				copyContext.Flush();
			}
		}
		assert(resource);
		return resource; 
	}
}