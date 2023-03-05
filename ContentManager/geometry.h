#pragma once
#include "content_common.h"
#include <common.h>
#include <vector>
#include <string>
#include <GeometricPrimitive.h>

namespace engine::content
{
	struct IMPORT_PARAMS
	{
		float smoothingAngle;
		bool bRH;
		bool bGenerateNormals;
		bool bGenerateTangents;
		bool bImportTextures;
		bool bImportAnimations;
	};

	struct VERTEX
	{
		Vec3 position;
		Vec3 normal;
		Vec4 tangent;
		Vec2 uv;
	};

	struct PK_STATIC_VERTEX
	{
		Vec3 position;
		uint8_t reserved[3];
		uint8_t t_sign;
		uint16_t normal[2];
		uint16_t tangent[2];
		Vec2 uv;
	};

	struct MESH
	{
		std::string name;


		// Generated
		std::vector<Vec3> vertices;
		std::vector<Vec3> normals;
		std::vector<Vec4> tangents;
		std::vector<std::vector<Vec2>> uvs;

		// Processed
		std::vector<uint32_t> indices;
		std::vector<VERTEX> processedVertices;

		std::vector<PK_STATIC_VERTEX> pkStaticVertices;
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

	enum PRIMITIVE_TYPE
	{
		Plane,
		Box,
		Sphere,
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
	
	

	struct GEOMETRY_DATA
	{
		void* buffer;
		size_t bufferSize;
		IMPORT_PARAMS params;
	};

	//CMANAGER_API Create
}
