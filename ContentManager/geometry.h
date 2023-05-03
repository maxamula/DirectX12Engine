#pragma once
#include "content_common.h"
#include <common.h>
#include <vector>
#include <string>
#include <GeometricPrimitive.h>

namespace engine::content
{
#pragma pack(push, 1)
	struct IMPORT_PARAMS
	{
		float smoothingAngle;
		bool bRH;
		bool bGenerateNormals;
		bool bGenerateTangents;
		bool bImportTextures;
		bool bImportAnimations;
		uint8_t pad[2];
	};
#pragma pack(pop)

	struct VERTEX
	{
		Vec3 position;
		Vec4 jointWeights;
		uint32_t jointIndices[4]{ uint32_invalid, uint32_invalid, uint32_invalid, uint32_invalid };
		Vec3 normal;
		Vec4 tangent;
		Vec2 uv;
		uint8_t red, green, blue;
		uint8_t pad;
	};

	namespace elements
	{
		enum ELEMENT_TYPE : uint32_t
		{
			POSITION = 1 << 0,
			NORMAL = 1 << 1,
			TANGENT = 1 << 2,
			COLOR = 1 << 3,
			TEXCOORD = 1 << 4,
			JOINT_WEIGHTS = 1 << 5,
			JOINT_INDICES = 1 << 6,
		};

		struct VERTEX_DATA_TYPE
		{
			enum type : uint32_t
			{
				FLOAT,
				FLOAT4,
				FLOAT3,
				FLOAT2,
				SINT,
				SINT4,
				SINT3,
				SINT2,
				UINT8,
				UINT16,
				UINT32,
				COUNT
			};
			constexpr static uint32_t size[] = { 4, 16, 12, 8, 4, 16, 12, 8, 1, 2, 4 };
			constexpr static DXGI_FORMAT format[] = { DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32_SINT, DXGI_FORMAT_R32G32B32A32_SINT, DXGI_FORMAT_R32G32B32_SINT, DXGI_FORMAT_R32G32_SINT, DXGI_FORMAT_R8_UINT, DXGI_FORMAT_R16_UINT, DXGI_FORMAT_R32_UINT };
			static_assert(_countof(size) == type::COUNT);
			static_assert(_countof(format) == type::COUNT);
		};
	}

	struct MESH
	{
		std::string name;

		// Generated
		std::vector<Vec3> vertices;
		std::vector<Vec3> normals;
		std::vector<Vec3> colors;
		std::vector<Vec4> tangents;
		std::vector<std::vector<Vec2>> uvs;
		std::vector<uint32_t> materialIndices;
		std::vector<uint32_t> materialUsed;

		// Processed
		std::vector<uint32_t> indices;
		std::vector<VERTEX> processedVertices;

		uint32_t elementMask = elements::ELEMENT_TYPE::POSITION;
		uint8_t* vertexData = nullptr;
		float lodThreshold = -1.0f;
		uint32_t lodId = uint32_invalid;
	};

	struct LOD_GROUP
	{
		std::string name;
		std::vector<MESH> meshes;
	};

	struct SCENE
	{
		std::string name;
		std::vector<LOD_GROUP> lod;
	};

	enum PRIMITIVE_TYPE : uint32_t
	{
		Plane,
		Box,
		Sphere,
		IcoSphere,
		Torus,
		Cylinder,
		Cone,
		Spring,
		Count
	};

	struct PRIMITIVE_DESC
	{
		PRIMITIVE_TYPE type;
		Vec3 size = {1, 1, 1};
		uint32_t segments[3] = { 1, 1, 1 };
		uint32_t lod = 0;
	};

	// Output data
	
	
#pragma pack(push, 1)
	struct GEOMETRY_DATA
	{
	public:
		void* buffer;
		uint64_t bufferSize;
	};
#pragma pack(pop)

	void DetermineMeshType(MESH& mesh);
	void PackData(const SCENE& scene, GEOMETRY_DATA& geometry);
	void ProcessScene(SCENE& scene, const IMPORT_PARAMS& params);
	CMANAGER_API void CreatePrimitive(PRIMITIVE_DESC& desc, IMPORT_PARAMS& params, GEOMETRY_DATA& data);
}
