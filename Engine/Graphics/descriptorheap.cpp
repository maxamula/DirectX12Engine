#include "descriptorheap.h"

namespace engine::gfx
{
	DescriptorHeap::DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, size_t capacity)
	{
		assert(device);
		m_cap = capacity;
		m_heap = std::make_unique<DirectX::DescriptorHeap>(device, type, flags, capacity);
		m_available = boost::dynamic_bitset<unsigned int>(capacity);
		m_available.set();
		m_shaderVisible = flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE ? true : false;
	}

	DescriptorHeap::~DescriptorHeap()
	{
		assert(m_heap.get() == NULL);
	}

	void DescriptorHeap::Release()
	{
		m_heap.reset();
	}

	DESCRIPTOR_HANDLE DescriptorHeap::Allocate()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		size_t index = _GetFreeIndex();
		assert(index != -1);
		m_available[index] = false;
		DESCRIPTOR_HANDLE handle;
		handle.index = index;
		handle.CPU = m_heap->GetCpuHandle(index);
		handle.GPU = m_shaderVisible ? m_heap->GetGpuHandle(index) : D3D12_GPU_DESCRIPTOR_HANDLE{};
		return handle;
	}

	void DescriptorHeap::Free(DESCRIPTOR_HANDLE handle)
	{
		if ((long long)handle.index == -1)
			return;
		std::lock_guard<std::mutex> lock(m_mutex);
		assert(handle.index < m_cap);
		m_available[handle.index] = true;
	}

	size_t DescriptorHeap::_GetFreeIndex() const
	{
		for (size_t i = 0; i < m_cap; i++)
		{
			if (m_available[i])
			{
				return i;
			}
		}
		return -1;
	}
}