#include "fbximporter.h"
#include "utils.h"

#pragma comment(lib, "libfbxsdk-md.lib")
#pragma comment(lib, "libxml2-md.lib")
#pragma comment(lib, "zlib-md.lib")

namespace engine::content
{
	namespace
	{
		std::mutex fbxMutex;
	} // Anon

	FbxContext::FbxContext(const char* file, SCENE* scene, IMPORT_PARAMS& params, GEOMETRY_DATA* data)
		: m_scene(scene), m_data(data), m_params(params)
	{
		assert(file);
		if (_Initialze())
		{
			_LoadFile(file);
		}
	}


	void FbxContext::GetScene(FbxNode* node)
	{
		assert_throw(m_fbxScene, "No scene loaded");
		if (!node)
			node = m_fbxScene->GetRootNode();
		if (!node)
			return;
		const int childCount = node->GetChildCount();
		for (int i = 0; i < childCount; i++)
		{
			FbxNode* child = node->GetChild(i);
			if(!child)
				continue;

			LOD_GROUP lod{};
			_GetMeshes(child, lod.meshes, 0, -1.0f);
			if (lod.meshes.size())
			{
				lod.name = lod.meshes[0].name;
				m_scene->lod.emplace_back(lod);
			}
		}
	}

	bool FbxContext::_Initialze()
	{
		m_manager = FbxManager::Create();
		if (!m_manager)
			return false;
		FbxIOSettings* ios = FbxIOSettings::Create(m_manager, IOSROOT);
		m_manager->SetIOSettings(ios);
		return true;
	}

	void FbxContext::_LoadFile(const char* file)
	{
		char sceneName[MAX_PATH];
		sprintf(sceneName, "imported_%S", file);
		m_fbxScene = FbxScene::Create(m_manager, sceneName);
		assert_throw(m_fbxScene, "Failed to create scene");
		FbxImporter* importer = FbxImporter::Create(m_manager, "importer");
		assert_throw(m_fbxScene, "Failed to create importer");
		bool bSuccess = importer->Initialize(file, -1, m_manager->GetIOSettings());
		assert_throw(bSuccess, "Failed to initialize importer");
		bSuccess = importer->Import(m_fbxScene);
		assert_throw(bSuccess, "Failed to import scene");
		importer->Destroy();
		m_scale = m_fbxScene->GetGlobalSettings().GetSystemUnit().GetConversionFactorTo(FbxSystemUnit::m);
	}

	void FbxContext::_GetMeshes(FbxNode* node, std::vector<MESH>& meshes, uint32_t lodId, float lodThreshold)
	{
		bool bLod = false;
		if (const int attribCount = node->GetNodeAttributeCount())
		{
			for (int i = 0; i < attribCount; i++)
			{
				FbxNodeAttribute* attrib = node->GetNodeAttributeByIndex(i);
				const FbxNodeAttribute::EType type = attrib->GetAttributeType();
				if (type == FbxNodeAttribute::eMesh)
				{
					_GetMesh(attrib, meshes, lodId, lodThreshold);
				}
				else if (type == FbxNodeAttribute::eLODGroup)
				{
					_GetLOD(attrib);
					bLod = true;
				}
			}
		}
		if (!bLod)
		{
			if (const int childCount = node->GetChildCount())
			{
				for (int i = 0; i < childCount; i++)
				{
					_GetMeshes(node->GetChild(i), meshes, lodId, lodThreshold);
				}
			}
		}
	}

	void FbxContext::_GetMesh(FbxNodeAttribute* attrib, std::vector<MESH>& meshes, uint32_t lodId, float lodThreshold)
	{
		FbxMesh* fbxMesh = (FbxMesh*)attrib;
		if (fbxMesh->RemoveBadPolygons() < 0)
			return;

		FbxGeometryConverter gc(m_manager);
		fbxMesh = (FbxMesh*)(gc.Triangulate(fbxMesh, true));
		if (!fbxMesh || fbxMesh->RemoveBadPolygons() < 0)
			return;
		FbxNode* node = fbxMesh->GetNode();

		MESH mesh{};
		mesh.lodId = lodId;
		mesh.lodThreshold = lodThreshold;
		mesh.name = (node->GetName()[0] != '\0') ? node->GetName() : fbxMesh->GetName();
		if (_GetMeshData(fbxMesh, mesh))
		{
			meshes.emplace_back(mesh);
		}	
	}

	bool FbxContext::_GetMeshData(FbxMesh* mesh, MESH& data)
	{
		const int polygonsCount = mesh->GetPolygonCount();
		if (polygonsCount <= 0)
			return false;
		const int vertexCount = mesh->GetControlPointsCount();
		FbxVector4* vertices = mesh->GetControlPoints();
		const int indexCount = mesh->GetPolygonVertexCount();
		int* indices = mesh->GetPolygonVertices();
		if (!indices || !indexCount)
			return false;
		data.indices.resize(indexCount);
		// join identical vertices
		std::vector vertexRef(vertexCount, uint32_invalid);

		for (int i = 0; i < indexCount; i++)
		{
			const uint32_t vIdx = indices[i];
			if (vertexRef[vIdx] != uint32_invalid)
			{
				data.indices[i] = vertexRef[vIdx];
			}
			else
			{
				FbxVector4 v = vertices[vIdx] * m_scale;
				data.indices[i] = (uint32_t)data.vertices.size();
				vertexRef[vIdx] = data.indices[i];
				data.vertices.emplace_back((float)v[0], (float)v[1], (float)v[2]);
			}
		}
		assert_throw(data.indices.size() % 3 == 0, "Mesh was not triangulated properly");
		// Materials
		FbxLayerElementArrayTemplate<int>* materialIndices;
		if (mesh->GetMaterialIndices(&materialIndices))
		{
			for (int i = 0; i < polygonsCount; i++)
			{
				const int materialIndex = materialIndices->GetAt(i);
				assert(materialIndex >= 0);
				data.materialIndices.emplace_back(materialIndex);
				if (std::find(data.materialUsed.begin(), data.materialUsed.end(), (uint32_t)materialIndex) == data.materialUsed.end())
				{
					data.materialUsed.emplace_back(materialIndex);
				}
			}
		}
		// Normals
		const bool bImportNormals = !m_params.bGenerateNormals;

		// Tanagents
		const bool bImportTangents = !m_params.bGenerateTangents;

		if (bImportNormals)
		{
			FbxArray<FbxVector4> normals;
			if (mesh->GenerateNormals() && mesh->GetPolygonVertexNormals(normals) && normals.Size() > 0)
			{
				const int normalsCount = normals.Size();
				for (int i = 0; i < normalsCount; i++)
				{
					data.normals.emplace_back((float)normals[i][0], (float)normals[i][1], (float)normals[i][2]);
				}
			}
			else
			{
				m_params.bGenerateNormals = true;
			}
		}

		if (bImportTangents)
		{
			FbxLayerElementArrayTemplate<FbxVector4>* tangents;
			if (mesh->GenerateTangentsData() && mesh->GetTangents(&tangents) && tangents->GetCount() > 0)
			{
				const int tangentsCount = tangents->GetCount();
				for (int i = 0; i < tangentsCount; i++)
				{
					FbxVector4 t = tangents->GetAt(i);
					data.tangents.emplace_back((float)t[0], (float)t[1], (float)t[2], (float)t[3]);
				}
			}
			else
			{
				m_params.bGenerateTangents = true;
			}
		}

		FbxStringList uvNames;
		mesh->GetUVSetNames(uvNames);
		const int uvCount = uvNames.GetCount();
		data.uvs.resize(uvCount);

		for (int i = 0; i < uvCount; i++)
		{
			FbxArray<FbxVector2> uvs;
			if (mesh->GetPolygonVertexUVs(uvNames.GetStringAt(i), uvs))
			{
				const int uvsCount = uvs.Size();
				for (int j = 0; j < uvsCount; j++)
				{
					data.uvs[i].emplace_back((float)uvs[j][0], (float)uvs[j][1]);
				}
			}
		}
		return true;
	}

	void FbxContext::_GetLOD(FbxNodeAttribute* attrib)
	{
		assert(attrib);
		FbxLODGroup* lodGroup = FbxCast<FbxLODGroup>(attrib);
		FbxNode* node = lodGroup->GetNode();
		LOD_GROUP lod{};
		lod.name = (node->GetName()[0] != '\0') ? node->GetName() : lodGroup->GetName();
		const int nodesCount = node->GetChildCount();
		for (int i = 0; i < nodesCount; i++)
		{
			float fThreshold = -1.0f;
			if (i > 0)
			{
				FbxDistance threshold;
				lodGroup->GetThreshold(i - 1, threshold);
				fThreshold = threshold.value() * m_scale;
			}
			_GetMeshes(node->GetChild(i), lod.meshes, lod.meshes.size(), fThreshold);
		}
		if (lod.meshes.size()) m_scene->lod.emplace_back(lod);
	}


	void ImportFbx(const char* file, IMPORT_PARAMS& params, GEOMETRY_DATA* data)
	{
		SCENE scene{};

		fbxMutex.lock();
		FbxContext context(file, &scene, params, data);
		context.GetScene();
		fbxMutex.unlock();
		context.Destroy();

		ProcessScene(scene, params);
		PackData(scene, *data);
	}
}

// TODO delete later
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{
	engine::content::IMPORT_PARAMS p{};
	p.bGenerateNormals = false;
	p.smoothingAngle = 60.0f;

	/*engine::content::PRIMITIVE_DESC d{};
	d.type = engine::content::PRIMITIVE_TYPE::Plane;
	d.segments[1] = 4;*/

	engine::content::GEOMETRY_DATA data{};
	//engine::content::ImportFbx("C:\\Users\\maxamula\\Desktop\\Crates.fbx", p, &data);
	return 0;
}