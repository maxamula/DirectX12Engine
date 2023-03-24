#pragma once
#include "graphics.h"

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
		inline ID3D12CommandQueue* GetCommandQueue() const { return m_cmdQueue; }
		inline ID3D12CommandAllocator* GetCommandAllocator() const { return m_cmdAlloc[m_iFrame]; }
		inline uint8_t CurrentBackBufferIndex() const { return m_iFrame; }
	private:
		void _WaitGPU(HANDLE event, ID3D12Fence1* pFence);	// wait if gpu is busy while executing commands

		ID3D12CommandQueue* m_cmdQueue = NULL;
		ID3D12GraphicsCommandList6* m_cmdList = NULL;
		ID3D12CommandAllocator* m_cmdAlloc[BACKBUFFER_COUNT];
		ID3D12Fence1* m_fence = NULL;
		HANDLE m_fenceEvent = NULL;
		uint64_t m_fenceValue = 0;
		uint8_t m_iFrame = 0;
	};
}

