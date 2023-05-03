#pragma once
#include "geometry.h"
#include <fbxsdk.h>


namespace engine::content
{
	class FbxContext
	{
	public:
		FbxContext(const char* file, SCENE* scene, IMPORT_PARAMS& params, GEOMETRY_DATA* data);

		void GetScene(FbxNode* node = nullptr);

		inline float GetScale() const { return m_scale; }
		inline void Destroy() { m_fbxScene->Destroy(); m_manager->Destroy(); }
	private:
		bool _Initialze();
		void _LoadFile(const char* file);

		
		void _GetMeshes(FbxNode* node, std::vector<MESH>& meshes, uint32_t lodId, float lodThreshold );
		void _GetMesh(FbxNodeAttribute* attrib, std::vector<MESH>& meshes, uint32_t lodId, float lodThreshold);
		bool _GetMeshData(FbxMesh* mesh, MESH& data);
		void _GetLOD(FbxNodeAttribute* attrib);


		SCENE* m_scene;
		GEOMETRY_DATA* m_data;
		IMPORT_PARAMS& m_params;
		FbxManager* m_manager;
		FbxScene* m_fbxScene;
		float m_scale = 1.0f;
	};

	CMANAGER_API void ImportFbx(const char* file, IMPORT_PARAMS& params, GEOMETRY_DATA* data);
}