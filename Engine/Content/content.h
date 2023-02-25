#pragma once

namespace engine::content
{
	struct RESOURCE_BLOB
	{
		uint64_t id;
		uint64_t size;
		uint8_t* data;
	};

	class Resource
	{
		virtual RESOURCE_BLOB Deserialize() = 0;
		virtual void LoadFromBlob(RESOURCE_BLOB blob) = 0;
	};

	void CreateAssetFile(LPCSTR szAssetFile, RESOURCE_BLOB* pBlobArray, uint64_t arraySize);
	void LoadContentBlobRange();
	uint64_t CreateResourceId();
}

