#include "pso.h"

namespace engine::gfx
{
	ID3D12PipelineState* CreatePSO(void* stream, uint64_t size)
	{
		assert_throw(stream && size, "Invalid PSO stream.");
		D3D12_PIPELINE_STATE_STREAM_DESC desc = {};
		desc.pPipelineStateSubobjectStream = stream;
		desc.SizeInBytes = size;
		return CreatePSO(desc);
	}

	ID3D12PipelineState* CreatePSO(D3D12_PIPELINE_STATE_STREAM_DESC desc)
	{
		ID3D12PipelineState* pso = nullptr;
		assert(device->CreatePipelineState(&desc, IID_PPV_ARGS(&pso)) == S_OK);
		return pso;
	}
}