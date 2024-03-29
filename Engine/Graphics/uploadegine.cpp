#include "uploadegine.h"
#include "buffers.h"

namespace engine::gfx::copy
{
	namespace
	{
		struct COPY_FRAME
		{
			ID3D12CommandAllocator* cmdAlloc = nullptr;
			ID3D12GraphicsCommandList6* cmdList = nullptr;
			ID3D12Resource* uploadBuffer = nullptr;
			void* mapped = nullptr;
			uint64_t fenceValue = 0;

			void WaitAndReset();
			void Release()
			{
				WaitAndReset();
				RELEASE(cmdAlloc);
				RELEASE(cmdList);
			}
			inline bool IsReady() const
			{
				return uploadBuffer == nullptr;
			}
		};
		COPY_FRAME g_copyFrames[COPY_FRAMES_COUNT]{};
		ID3D12CommandQueue* g_copyQueue = nullptr;
		ID3D12Fence1* g_copyFence = nullptr;
		uint64_t g_copyFenceValue = 0;
		HANDLE g_copyFenceEvent = nullptr;
		std::mutex g_copyMutex;
		std::mutex g_queueMutex;
		std::condition_variable g_frameAvailable;

		void COPY_FRAME::WaitAndReset()
		{
			if (g_copyFence->GetCompletedValue() < g_copyFenceValue)
			{
				ThrowIfFailed(g_copyFence->SetEventOnCompletion(g_copyFenceValue++, g_copyFenceEvent));
				WaitForSingleObject(g_copyFenceEvent, INFINITE);
			}
			RELEASE(uploadBuffer);
			mapped = nullptr;
			g_frameAvailable.notify_one();
		}
	}

	void Initialize()
	{
		LOG_TRACE("Initializing upload engine...");
		DEVICE_CHECK;
		for (uint32_t i = 0; i < COPY_FRAMES_COUNT; i++)
		{
			COPY_FRAME& frame = g_copyFrames[i];
			ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&frame.cmdAlloc)));
			INDEXED_NAME_BUFFER(40);
			SET_NAME_INDEXED(frame.cmdAlloc, L"Copy command allocator", i);
			ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, frame.cmdAlloc, nullptr, IID_PPV_ARGS(&frame.cmdList)));
			SET_NAME_INDEXED(frame.cmdList, L"Copy command list", i);
			ThrowIfFailed(frame.cmdList->Close());
		}

		D3D12_COMMAND_QUEUE_DESC desc{};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

		ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_copyQueue)));
		SET_NAME(g_copyQueue, L"Copy command queue");

		ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_copyFence)));
		SET_NAME(g_copyFence, L"Copy fence");
		g_copyFenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		assert_throw(g_copyFenceEvent);
	}

	void Shutdown()
	{
		for(uint32_t i = 0; i < COPY_FRAMES_COUNT; i++)
		{
			g_copyFrames[i].Release();
		}

		if (g_copyFenceEvent)
		{
			CloseHandle(g_copyFenceEvent);
			g_copyFenceEvent = nullptr;
		}

		RELEASE(g_copyQueue);
		RELEASE(g_copyFence);
		g_copyFenceValue = 0;
	}

	uint32_t GetAvailableFrame(std::unique_lock<std::mutex>& lock)
	{
		uint32_t index = uint32_invalid;
		const uint32_t count = COPY_FRAMES_COUNT;
		COPY_FRAME* const frames = &g_copyFrames[0];
		SearchForAvailableFrame:
		for (uint32_t i = 0; i < count; i++)
		{
			if (frames[i].IsReady())
			{
				index = i;
				break;
			}
		}

		if (index == uint32_invalid)
		{
			g_frameAvailable.wait(lock);
			goto SearchForAvailableFrame;
		}
		else
		{
			return index;
		}
	}

	CopyContext::CopyContext(uint32_t alignedSize)
	{
		{
			std::unique_lock lock(g_copyMutex);
			const uint32_t availableFrame = GetAvailableFrame(lock);
			assert(availableFrame != uint32_invalid);
			g_copyFrames[availableFrame].uploadBuffer = (ID3D12Resource*)1;	// Just to mark it as busy
		}

		COPY_FRAME& frame = g_copyFrames[m_frameIndex];
		frame.uploadBuffer = gfx::CreateBuffer(alignedSize, nullptr, true);	// cpu visible buffer
		INDEXED_NAME_BUFFER(40);
		SET_NAME_INDEXED(frame.uploadBuffer, L"Upload buffer, Size", alignedSize);

		const D3D12_RANGE range{};
		ThrowIfFailed(frame.uploadBuffer->Map(0, &range, &frame.mapped));
		assert(frame.mapped);
		m_cmdList = frame.cmdList;
		m_uploadBuffer = frame.uploadBuffer;
		m_mapped = frame.mapped;
		
		ThrowIfFailed(frame.cmdAlloc->Reset());
		ThrowIfFailed(frame.cmdList->Reset(frame.cmdAlloc, nullptr));
	}

	void CopyContext::Flush()
	{
		COPY_FRAME& frame = g_copyFrames[m_frameIndex];
		ID3D12GraphicsCommandList6* cmd = frame.cmdList;
		ThrowIfFailed(cmd->Close());
		ID3D12CommandList* const cmdLists[] = { cmd };
		std::lock_guard lock(g_queueMutex);
		g_copyQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
		g_copyFenceValue++;
		frame.fenceValue = g_copyFenceValue;
		ThrowIfFailed(g_copyQueue->Signal(g_copyFence, g_copyFenceValue));
		frame.WaitAndReset();
		m_cmdList = nullptr;
		m_uploadBuffer = nullptr;
		m_mapped = nullptr;
		m_frameIndex = uint32_invalid;
	}
}