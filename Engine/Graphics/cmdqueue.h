#pragma once
#include "graphics.h"
#include "utils.h"

namespace engine::gfx
{
	class CommandQueue
	{
	public:
		CommandQueue() = default;
		CommandQueue(D3D12_COMMAND_LIST_TYPE type);
		DISABLE_MOVE_COPY(CommandQueue);
		~CommandQueue();
		void BeginFrame();
		void EndFrame();
		void Flush();
		void Release();	
		// Accessors
		[[nodiscard]] inline ID3D12CommandQueue* GetCommandQueue() const { return m_cmdQueue; }
		[[nodiscard]] inline ID3D12CommandAllocator* GetCommandAllocator() const { return m_cmdAlloc[m_frame]; }
		[[nodiscard]] inline uint8_t FramerIndex() const { return m_frame; }
		[[nodiscard]] inline ID3D12GraphicsCommandList6* GetCommandList() const { return m_cmdList; }

		utils::Event<> DeferedReleaseEvent; // event to release resources that are used by gpu
	private:
		void _WaitGPU(HANDLE event, ID3D12Fence1* pFence);	// wait if gpu is busy while executing commands

		ID3D12CommandQueue* m_cmdQueue = nullptr;
		ID3D12GraphicsCommandList6* m_cmdList = nullptr;
		ID3D12CommandAllocator* m_cmdAlloc[BACKBUFFER_COUNT];
		ID3D12Fence1* m_fence = nullptr;
		HANDLE m_fenceEvent = nullptr;
		uint64_t m_fenceValue = 0;
		uint8_t m_frame = 0;
	};
}

