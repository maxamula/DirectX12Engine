#pragma once
#include <DirectXMath.h>

namespace engine
{
	struct COMPONENT_TYPE
	{
		enum type
		{
			TRANSFORM,
			MESH,
			MATERIAL,
			LIGHT,
			CAMERA,
			ANIMATION,
			SCRIPT,
			COUNT
		};
	};

	struct Transformation
	{
		DirectX::XMFLOAT3 position = { 90.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 rotation = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
		[[nodiscard]] DirectX::XMMATRIX GetMatrix() const
		{
			return DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
				DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
				DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		}
	};
}
