#pragma once
#include "graphics.h"

namespace engine::gfx
{
	struct DescriptorRange : public D3D12_DESCRIPTOR_RANGE1
	{
		constexpr DescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE rangeType, uint32_t numDescriptors, uint32_t shaderRegister, uint32_t space = 0,
			D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE, uint32_t tableOffset = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND)
			: D3D12_DESCRIPTOR_RANGE1{ rangeType, numDescriptors, shaderRegister, space, flags, tableOffset }
		{

		}
	};

	class RootParameter : public D3D12_ROOT_PARAMETER1
	{
	public:
		inline void AsConstants(uint32_t numConstants, D3D12_SHADER_VISIBILITY visibility, uint32_t shaderRegister, uint32_t space = 0)
		{
			ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			ShaderVisibility = visibility;
			Constants.Num32BitValues = numConstants;
			Constants.ShaderRegister = shaderRegister;
			Constants.RegisterSpace = space;
		}

		inline void AsDescriptor(D3D12_ROOT_PARAMETER_TYPE type, D3D12_SHADER_VISIBILITY visibility, uint32_t shaderRegister, uint32_t space = 0,
			D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE)
		{
			ParameterType = type;
			ShaderVisibility = visibility;
		}

		inline void AsDescriptorTable(D3D12_SHADER_VISIBILITY visibility, DescriptorRange* ranges, uint32_t numRanges)
		{
			ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			ShaderVisibility = visibility;
			DescriptorTable.NumDescriptorRanges = numRanges;
			DescriptorTable.pDescriptorRanges = ranges;
		}
	};

	class RootSignature : public D3D12_ROOT_SIGNATURE_DESC1
	{
	public:
		explicit RootSignature(RootParameter* params, uint32_t numParams, const D3D12_STATIC_SAMPLER_DESC* staticSamplers = NULL, uint32_t numSamplers = 0,
			D3D12_ROOT_SIGNATURE_FLAGS flags =
			D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS
		)
			: D3D12_ROOT_SIGNATURE_DESC1{ numParams, params, numSamplers, staticSamplers, flags }
		{}

		inline ID3D12RootSignature* Create() const
		{
			const D3D12_ROOT_SIGNATURE_DESC1& desc{ *this };
			D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionedDesc{};
			versionedDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
			versionedDesc.Desc_1_1 = desc;

			ID3DBlob* sigBlob;
			ID3DBlob* errorBlob;
			if (FAILED(D3D12SerializeVersionedRootSignature(&versionedDesc, &sigBlob, &errorBlob)))
			{
				const char* errormsg = errorBlob ? (const char*)errorBlob->GetBufferPointer() : "";
				OutputDebugStringA(errormsg);
				return nullptr;
			};
			ID3D12RootSignature* signature = NULL;
			assert(device->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&signature)) == S_OK);
			RELEASE(sigBlob);
			RELEASE(errorBlob);
			return signature;
		}
	};
}
