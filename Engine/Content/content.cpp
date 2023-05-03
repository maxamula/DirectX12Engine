#include "content.h"
#include "utils.h"
#include "../Graphics/content.h"
#include <fstream>

namespace engine::content
{
	namespace
	{
		utils::ItemManager<uint8_t*> g_geometryHierarchies;
		std::mutex g_geometryHierarchiesMutex;

		class GeometryHierarchyStream
		{
		public:
			struct LOD_OFFSET
			{
				uint16_t offset;
				uint16_t count;
			};

			DISABLE_MOVE_COPY(GeometryHierarchyStream);
			GeometryHierarchyStream(uint8_t* const buffer, uint32_t lods = uint32_invalid)
				: m_buffer(buffer)
			{
				if (lods != uint32_invalid)
				{
					*((uint32_t*)buffer) = lods;
				}
				m_lodCount = lods;
				m_thresholds = (float*)(&buffer[sizeof(uint32_t)]);
				m_lodOffsets = (LOD_OFFSET*)(&m_thresholds[m_lodCount]);
				m_gpuHandles = (uint64_t*)(&m_lodOffsets[m_lodCount]);
			}

			void GPUHandles(uint32_t lod, uint64_t*& ids, uint32_t& idCount)
			{
				assert_throw(lod <  m_lodCount);
				ids = (uint64_t*)(m_buffer + m_lodOffsets[lod].offset);
				idCount = m_lodOffsets[lod].count;
			}

			uint32_t LODFromThreshold(float threshold)
			{
				for (uint32_t i = m_lodCount - 1; i > 0; i--)
				{
					if (m_thresholds[i] <= threshold) return i;
				}
				assert(false);
				return 0;
			}

			// Accessors
			[[nodiscard]] uint32_t LODCount() const { return m_lodCount; }
			[[nodiscard]] float* Thresholds() const { return m_thresholds; }
			[[nodiscard]] LOD_OFFSET* LODOffsets() const { return m_lodOffsets; }
			[[nodiscard]] uint64_t* GPUHandles() const { return m_gpuHandles; }

		private:
			uint8_t* const m_buffer = nullptr;
			float* m_thresholds = nullptr;
			LOD_OFFSET* m_lodOffsets = nullptr;
			uint64_t* m_gpuHandles = nullptr;
			uint32_t m_lodCount = 0;
		};

		uint32_t GetGeometryHierarchyBufferSize(const void* const data)
		{
			utils::BinaryReader br((uint8_t*)data);
			const uint32_t lodCount = br.ReadUInt32(); // LOD count
			assert_throw(lodCount);
			uint32_t size = sizeof(uint32_t) + sizeof(float) + sizeof(GeometryHierarchyStream::LOD_OFFSET) * lodCount;
			for (uint32_t i = 0; i < lodCount; i++)
			{
				br.Skip(sizeof(uint32_t));
				size += sizeof(uint64_t) * br.ReadUInt32();
				br.Skip(br.ReadUInt32());
			}
			return size;
		}

		uint64_t CreateMeshHierarhy(const void* const data)
		{
			const uint32_t size = GetGeometryHierarchyBufferSize(data);
			uint8_t* const buffer = (uint8_t* const)malloc(size);
			utils::BinaryReader br((uint8_t*)data);
			const uint32_t lodCount = br.ReadUInt32(); // LOD count
			GeometryHierarchyStream writer(buffer, lodCount);
			uint16_t submeshIdx = 0;
			uint64_t* const gpuHandles = writer.GPUHandles();

			for (uint32_t lodIdx = 0; lodIdx < lodCount; lodIdx++)
			{
				writer.Thresholds()[lodIdx] = br.ReadFloat();
				const uint32_t idCount = br.ReadUInt32();
				writer.LODOffsets()[lodIdx].offset = {submeshIdx, (uint16_t)idCount};
				br.Skip(sizeof(uint32_t)); // skip size of submeshes
				for (uint32_t idIdx = 0; idIdx < idCount; idIdx++)
				{
					const uint8_t* at = (uint8_t*)br.CurrentReadPos();
					gpuHandles[submeshIdx++] = gfx::submesh::Add(at);
					br.Skip((uint64_t)(at - br.CurrentReadPos()));
				}
			}

			std::lock_guard lock(g_geometryHierarchiesMutex);
			return g_geometryHierarchies.Add(buffer);
		}

		uint64_t CreateGeometryResource(const void* const data)
		{
			return CreateMeshHierarhy(data);
		}


		uint64_t RemoveGeometryResource(uint64_t handle)
		{
			std::lock_guard lock(g_geometryHierarchiesMutex);
			uint8_t* const buffer = g_geometryHierarchies.Get(handle);

			GeometryHierarchyStream stream(buffer);
			const uint32_t lodCount = stream.LODCount();
			uint32_t idIdx = 0;
			for (uint32_t lod = 0; lod < lodCount; lod++)
			{
				const uint32_t idCount = stream.LODOffsets()[lod].count;
				for (uint32_t i = 0; i < idCount; i++)
				{
					gfx::submesh::Remove(stream.GPUHandles()[idIdx++]);
				}
			}
		}
	}

	uint64_t CreateResourse(const void* const data, ASSET_TYPE type)
	{
		uint64_t id = UINT64_MAX;
		switch (type)
		{
		case ASSET_TYPE::ANIMATION:
			break;
		case ASSET_TYPE::AUDIO:
			break;
		case ASSET_TYPE::MATERIAL:
			break;
		case ASSET_TYPE::MESH:
			CreateGeometryResource(data);
			break;
		case ASSET_TYPE::SKELETON:
			break;
		case ASSET_TYPE::TEXTURE:
			break;
		}
		assert(id != UINT64_MAX);
		return id;
	}

	void ReleaseResourse(uint64_t handle, ASSET_TYPE type)
	{
		switch (type)
		{
		case ASSET_TYPE::ANIMATION:
			break;
		case ASSET_TYPE::AUDIO:
			break;
		case ASSET_TYPE::MATERIAL:
			break;
		case ASSET_TYPE::MESH:
			RemoveGeometryResource(handle);
			break;
		case ASSET_TYPE::SKELETON:
			break;
		case ASSET_TYPE::TEXTURE:
			break;
		}
	}
}