#pragma once

namespace engine::content
{
	enum ASSET_TYPE
	{
		UNKNOWN,
		ANIMATION,
		AUDIO,
		MATERIAL,
		MESH,
		SKELETON,
		TEXTURE,
		COUNT
	};

	enum PRIMITIVE_TOPOLOGY
	{

	};

	uint64_t CreateResourse(const void* const data, ASSET_TYPE type);
	void ReleaseResourse(uint64_t handle, ASSET_TYPE type);
}

