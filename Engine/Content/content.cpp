#include "content.h"
#include <fstream>

namespace engine::content
{
	namespace
	{
		struct RESOURCE_INFO
		{
			uint64_t id;
			uint64_t size;
			uint64_t offset;
		};
	}
	void CreateAssetFile(LPCSTR szAssetFile, RESOURCE_BLOB* pBlobArray, uint64_t arraySize)
	{
		//// create or open file
		//std::ofstream file(szAssetFile, std::ios::binary | std::ios::out | std::ios::trunc);
		//// determine size of content blob
		//uint64_t blobSize = sizeof(uint64_t) + (arraySize * sizeof(RESOURCE_INFO));
		//for (int i = 0; i < arraySize; i++)
		//	blobSize += pBlobArray[i].size;
		//// allocate memory
		//uint8_t* pBlob = new uint8_t[blobSize];
		//uint64_t contentSize = 0;
		//uint8_t* at = pBlob;
		//// Write num resources
		//WRITEMEM(arraySize);
		//// Create resource table
		//for (int i = 0; i < arraySize; i++)
		//{
		//	RESOURCE_INFO info = { pBlobArray[i].id, pBlobArray[i].size, sizeof(uint64_t) + (arraySize * sizeof(RESOURCE_INFO) + contentSize) };
		//	WRITEMEM(info);
		//	contentSize += pBlobArray[i].size;
		//}
		//// write binary data
		//for (int i = 0; i < arraySize; i++)
		//{
		//	memcpy((void*)at, pBlobArray[i].data, pBlobArray[i].size);
		//	at += pBlobArray[i].size;
		//}
		//// write to file
		//file.write((char*)pBlob, blobSize);
		//// cleanup
		//delete[] pBlob;
	}

	void LoadContentBlobRange()
	{
	}
	uint64_t CreateResourceId()
	{
		return 0;
	}
}