#include "cmdqueue.h"

namespace engine::gfx
{
	CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		DEVICE_CHECK;
		// Create command queue
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
		cmdQueueDesc.Type = type;
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue));
		m_cmdQueue->SetName(L"Main command queue");
		// Create command allocator
		for (int i = 0; i < BACKBUFFER_COUNT; ++i)
		{
			device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_cmdAlloc[i]));
			wchar_t name[25];
			swprintf_s(name, L"Command allocator (%d)", i);
		}	
		// Create command list
		device->CreateCommandList(0, type, m_cmdAlloc[0], nullptr, IID_PPV_ARGS(&m_cmdList));
		m_cmdList->SetName(L"Main command list");
		m_cmdList->Close();
		// Create fence
		device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
		m_fenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
	}

	CommandQueue::~CommandQueue()
	{
		assert_throw(!m_cmdQueue, "Command queue wasn't released properly.");
	}

	void CommandQueue::BeginFrame()
	{
		_WaitGPU(m_fenceEvent, m_fence);
		// Get command allocator
		m_cmdAlloc[m_iFrame]->Reset();
		// Get command list
		m_cmdList->Reset(m_cmdAlloc[m_iFrame], nullptr);
	}

	void CommandQueue::EndFrame()
	{
		// Close command list
		m_cmdList->Close();
		// Execute command list
		ID3D12CommandList* cmdLists[] = { m_cmdList };
		m_cmdQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
		// Signal fence
		m_cmdQueue->Signal(m_fence, ++m_fenceValue);
		// Increment frame index
		m_iFrame = (m_iFrame + 1) % BACKBUFFER_COUNT;
	}

	void CommandQueue::Flush()
	{
		// Flush for each frame
		for (int i = 0; i < BACKBUFFER_COUNT; ++i)
		{
			// Signal fence
			m_cmdQueue->Signal(m_fence, ++m_fenceValue);
			// Wait for fence
			_WaitGPU(m_fenceEvent, m_fence);
		}
	}

	void CommandQueue::Release()
	{
		Flush();
		CloseHandle(m_fenceEvent);
		RELEASE(m_cmdQueue);
		RELEASE(m_cmdList);
		for (int i = 0; i < BACKBUFFER_COUNT; ++i)
			RELEASE(m_cmdAlloc[i]);
		RELEASE(m_fence);
	}

	void CommandQueue::_WaitGPU(HANDLE event, ID3D12Fence1* pFence)
	{
		if (pFence->GetCompletedValue() < m_fenceValue)
		{
			pFence->SetEventOnCompletion(m_fenceValue, event);
			WaitForSingleObject(event, INFINITE);
		}
	}
}