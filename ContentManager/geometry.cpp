#include "geometry.h"
#include "generator/PlaneMesh.hpp"
#include "generator/BoxMesh.hpp"
#include "generator/IcoSphereMesh.hpp"
#include "generator/SphereMesh.hpp"
#include "generator/SpringMesh.hpp"
#include "generator/TorusMesh.hpp"
#include "generator/CylinderMesh.hpp"
#include "generator/AnyGenerator.hpp"
#include "utils.h"
#include <DirectXMath.h>

#ifdef _DEBUG
#pragma comment(lib, "dbggenerator.lib")
#else
#pragma comment(lib, "generator.lib")
#endif // _DEBUG



using namespace DirectX;

namespace engine::content
{
	void DetermineMeshType(MESH& mesh)
	{
		if (mesh.normals.size())
			mesh.elementMask |= elements::ELEMENT_TYPE::NORMAL;
		if (mesh.colors.size())
			mesh.elementMask |= elements::ELEMENT_TYPE::COLOR;
		if (mesh.tangents.size())
			mesh.elementMask |= elements::ELEMENT_TYPE::TANGENT;
		if (mesh.uvs.size() && mesh.uvs[0].size())
			mesh.elementMask |= elements::ELEMENT_TYPE::TEXCOORD;
		// TODO add skeletal mesh support
	}
	namespace
	{
		void GeneratePrimitive(SCENE& scene, PRIMITIVE_DESC& desc)
		{
			LOD_GROUP lod;
			lod.name = "LOD0";
			MESH mesh;
			std::function fnWriteTriangleData = [&](generator::AnyGenerator<generator::Triangle> gen)
			{
				while (!gen.done())
				{
					generator::Triangle triangle = gen.generate();
					mesh.indices.emplace_back(triangle.vertices[0]);
					mesh.indices.emplace_back(triangle.vertices[1]);
					mesh.indices.emplace_back(triangle.vertices[2]);
					gen.next();
				}
			};

			std::function fnWriteVertexData = [&](generator::AnyGenerator<generator::MeshVertex> gen)
			{
				std::vector<Vec2> uv0;
				while (!gen.done())
				{
					generator::MeshVertex vertex = gen.generate();
					mesh.vertices.emplace_back(vertex.position[0], vertex.position[1], vertex.position[2]);
					uv0.emplace_back(vertex.texCoord[0], vertex.texCoord[1]);
					gen.next();
				}
				mesh.uvs.push_back(uv0);
			};

			switch (desc.type)
			{
			case PRIMITIVE_TYPE::Plane:
			{
				auto generated = generator::PlaneMesh({ (desc.size.x), desc.size.y }, { (int)desc.segments[0], (int)desc.segments[1] });
				fnWriteTriangleData(generated.triangles());
				fnWriteVertexData(generated.vertices());
				break;
			}
			case PRIMITIVE_TYPE::Box:
			{
				auto generated = generator::BoxMesh({ desc.size.x, desc.size.y, desc.size.z }, { (int)desc.segments[0], (int)desc.segments[1], (int)desc.segments[2] });
				fnWriteTriangleData(generated.triangles());
				fnWriteVertexData(generated.vertices());
				break;
			}
			case PRIMITIVE_TYPE::Sphere:
			{
				auto generated = generator::SphereMesh(desc.size.x, (int)desc.segments[0], (int)desc.segments[1]);
				fnWriteTriangleData(generated.triangles());
				fnWriteVertexData(generated.vertices());
				break;
			}
			case PRIMITIVE_TYPE::IcoSphere:
			{
				auto generated = generator::IcoSphereMesh(desc.size.x, (int)desc.segments[0]);
				fnWriteTriangleData(generated.triangles());
				fnWriteVertexData(generated.vertices());
				break;
			}
			case PRIMITIVE_TYPE::Torus:
			{
				auto generated = generator::TorusMesh(desc.size.x, desc.size.y, (int)desc.segments[0], (int)desc.segments[1]);
				fnWriteTriangleData(generated.triangles());
				fnWriteVertexData(generated.vertices());
				break;
			}
			case PRIMITIVE_TYPE::Cylinder:
			{
				auto generated = generator::CylinderMesh(desc.size.x, desc.size.y, (int)desc.segments[0], (int)desc.segments[1]);
				fnWriteTriangleData(generated.triangles());
				fnWriteVertexData(generated.vertices());
				break;
			}
			}
			lod.meshes.push_back(mesh);
			scene.lod.push_back(lod);
		}
		
		uint32_t GetPackedVertexSize(elements::ELEMENT_TYPE mask)
		{
			uint32_t size = sizeof(Vec3); // Positions
			if (mask & elements::ELEMENT_TYPE::NORMAL)
				size += sizeof(Vec3);
			if (mask & elements::ELEMENT_TYPE::TANGENT)
				size += sizeof(Vec4);
			if (mask & elements::ELEMENT_TYPE::COLOR)
				size += sizeof(uint8_t) * 3;
			if (mask & elements::ELEMENT_TYPE::TEXCOORD)
				size += sizeof(Vec2);
			// TODO Skeletal joint data
			return size;
		}

		void ProcessVertices(MESH& mesh, const IMPORT_PARAMS& params)
		{
			assert((mesh.indices.size() % 3) == 0);
			if (params.bGenerateNormals || mesh.normals.empty())
			{
				const uint32_t nIndices = (uint32_t)mesh.indices.size();
				mesh.normals.resize(nIndices);
				for (uint32_t i = 0; i < nIndices; i++)
				{
					const uint32_t& i0 = mesh.indices[i];
					const uint32_t& i1 = mesh.indices[++i];
					const uint32_t& i2 = mesh.indices[++i];
					XMVECTOR v0 = XMLoadFloat3((XMFLOAT3*)&mesh.vertices[i0]);
					XMVECTOR v1 = XMLoadFloat3((XMFLOAT3*)&mesh.vertices[i1]);
					XMVECTOR v2 = XMLoadFloat3((XMFLOAT3*)&mesh.vertices[i2]);

					XMVECTOR edge1 = XMVectorSubtract(v1, v0);
					XMVECTOR edge2 = XMVectorSubtract(v2, v0);
					XMVECTOR normal = XMVector3Normalize(XMVector3Cross(edge1, edge2));

					XMStoreFloat3((XMFLOAT3*)&mesh.normals[i], normal);
					mesh.normals[i - 1] = mesh.normals[i];
					mesh.normals[i - 2] = mesh.normals[i];
				}
			}
			// process normals
			const float cosAngle = XMScalarCos((180.0f - params.smoothingAngle) * XM_PI / 180.0f);
			const uint32_t nVertices = (uint32_t)mesh.vertices.size();
			const uint32_t nIndices = (uint32_t)mesh.indices.size();
			assert(nVertices && nIndices);
			std::vector<std::vector<uint32_t>> vertexToIndices;
			vertexToIndices.resize(nVertices);
			for (uint32_t i = 0; i < nIndices; i++)
				vertexToIndices[mesh.indices[i]].emplace_back(i);

			std::vector<uint32_t> indices;
			indices.resize(nIndices);
			std::vector<VERTEX> vertices;
			for (uint32_t i = 0; i < nVertices; i++)
			{
				std::vector<uint32_t> refs = vertexToIndices[i];
				uint32_t nRefs = (uint32_t)refs.size();

				for (uint32_t j = 0; j < nRefs; j++)
				{
					indices[refs[j]] = vertices.size();
					VERTEX& v = vertices.emplace_back();
					v.position = mesh.vertices[mesh.indices[refs[j]]];

					XMVECTOR normal1 = XMLoadFloat3((XMFLOAT3*)&mesh.normals[refs[j]]);
					for (uint32_t k = j + 1; k < nRefs; k++)
					{
						float cos = 0.0f;
						XMVECTOR normal2 = XMLoadFloat3((XMFLOAT3*)&mesh.normals[refs[k]]);
						XMStoreFloat(&cos, XMVector3Dot(normal1, normal2) / XMVector3Length(normal1));
						if (cos >= cosAngle)
						{
							normal1 += normal2;
							indices[refs[k]] = indices[refs[j]];
							refs.erase(refs.begin() + k);
							k--;
							nRefs--;
						}
					}
					XMStoreFloat3((XMFLOAT3*)&v.normal, XMVector3Normalize(normal1));
				}
			}
			mesh.processedVertices = vertices;
			mesh.indices = indices;
			DetermineMeshType(mesh);
		}

		void ProcessUVs(MESH& mesh)
		{
			std::vector<VERTEX> oldVertices;
			oldVertices.swap(mesh.processedVertices);
			std::vector<uint32_t> oldIndices;
			oldIndices.swap(mesh.indices);

		}

		bool SplitByMaterial(uint32_t materialIdx, MESH& mesh, MESH& submesh)
		{
			submesh.name = mesh.name;
			submesh.lodThreshold = mesh.lodThreshold;
			submesh.lodId = mesh.lodId;
			submesh.materialUsed.emplace_back(materialIdx);
			submesh.uvs.resize(mesh.uvs.size());

			const uint32_t polygonCount = (uint32_t)mesh.indices.size() / 3;
			std::vector<uint32_t> vertexRef(mesh.vertices.size(), uint32_invalid);

			for (uint32_t i = 0; i < polygonCount; i++)
			{
				const uint32_t matIdx = mesh.materialIndices[i];
				if (matIdx != materialIdx) continue;

				const uint32_t index = i * 3;
				for (uint32_t j = index; j < index + 3; j++)
				{
					const uint32_t vIdx = mesh.indices[j];
					if (vertexRef[vIdx] != uint32_invalid)
					{
						submesh.indices.emplace_back(vertexRef[vIdx]);
					}
					else
					{
						submesh.indices.emplace_back(submesh.vertices.size());
						vertexRef[vIdx] = submesh.indices.back();
						submesh.vertices.emplace_back(mesh.vertices[vIdx]);
					}

					if (mesh.normals.size())
					{
						submesh.normals.emplace_back(mesh.normals[j]);
					}
					
					if (mesh.tangents.size())
					{
						submesh.tangents.emplace_back(mesh.tangents[j]);
					}

					for (uint32_t k = 0; k < mesh.uvs.size(); k++)
					{
						if(mesh.uvs[k].size())
							submesh.uvs[k].emplace_back(mesh.uvs[k][j]);
					}
				}
			}
			return submesh.indices.size() > 0;
		}

		void SplitByMaterial(SCENE& scene)
		{
			for (auto& lod : scene.lod)
			{
				std::vector<MESH> newMeshes;

				for (auto& m : lod.meshes)
				{
					const uint32_t materialsCount = (uint32_t)m.materialUsed.size();
					if (materialsCount > 1)
					{
						for (uint32_t i = 0; i < materialsCount; i++)
						{
							MESH submesh;
							if(SplitByMaterial(m.materialUsed[i], m, submesh))
								newMeshes.emplace_back(submesh);
						}
					}
					else
					{
						newMeshes.emplace_back(m);
					}
				}
				lod.meshes.swap(newMeshes);
			}
		}

		size_t GetMeshSize(const MESH& mesh)
		{
			const uint64_t nVertices = mesh.processedVertices.size();
			const uint64_t vertexBufSize = nVertices * GetPackedVertexSize((elements::ELEMENT_TYPE)mesh.elementMask);
			const uint64_t indexSize = (nVertices < (1 << 16)) ? sizeof(uint16_t) : sizeof(uint32_t);
			const uint64_t indexBufferSize = mesh.indices.size() * indexSize;
			uint64_t size
			{
				sizeof(uint32_t) +
				mesh.name.size() +
				sizeof(uint32_t) +	// vertex component mask
				sizeof(uint32_t) +	// lod id
				sizeof(uint32_t) +	// vertex size
				sizeof(uint32_t) + // num vertices
				sizeof(uint32_t) + // index size
				sizeof(uint32_t) + // num indices
				sizeof(float) + // LOD threshold
				vertexBufSize +
				indexBufferSize
			};
			return size;
		}

		uint64_t GetPackedBufferSize(const SCENE& scene)
		{
			uint64_t size
			{
				sizeof(uint32_t) +	// num chars
				scene.name.size() +	// name chars
				sizeof(uint32_t)  	// num lod
			};

			for (auto& lod : scene.lod)
			{
				uint64_t lodSize
				{
					sizeof(uint32_t) +
					lod.name.size() +
					sizeof(uint32_t) 	// num meshes	
				};

				for (auto& mesh : lod.meshes)
				{
					lodSize += GetMeshSize(mesh);
				}
				size += lodSize;
			}
			return size;
		}
	}

	void PackVertices(const MESH& mesh, utils::BinaryWriter& buf)
	{
		const uint32_t verticesCount = (uint32_t)mesh.processedVertices.size();

		for (uint32_t i = 0; i < verticesCount; i++)
		{
			buf.WriteBytes(mesh.processedVertices[i].position);
			if (mesh.elementMask & elements::ELEMENT_TYPE::NORMAL) buf.WriteBytes(mesh.processedVertices[i].normal);
			if (mesh.elementMask & elements::ELEMENT_TYPE::TANGENT) buf.WriteBytes(mesh.processedVertices[i].tangent);
			if (mesh.elementMask & elements::ELEMENT_TYPE::TEXCOORD) buf.WriteBytes(mesh.processedVertices[i].uv);
			if (mesh.elementMask & elements::ELEMENT_TYPE::COLOR)
			{
				buf.WriteBytes(mesh.processedVertices[i].red);
				buf.WriteBytes(mesh.processedVertices[i].green);
				buf.WriteBytes(mesh.processedVertices[i].blue);
			}
		}
	}

	void PackData(const SCENE& scene, GEOMETRY_DATA& geometry)
	{
		uint64_t bufSize = GetPackedBufferSize(scene);
		geometry.bufferSize = bufSize;
		geometry.buffer = CoTaskMemAlloc(bufSize);

		utils::BinaryWriter writer(geometry.buffer);
		writer.WriteUInt32(scene.name.size());
		writer.WriteString(scene.name);
		writer.WriteUInt32(scene.lod.size());
		for (auto& lod : scene.lod)
		{
			writer.WriteUInt32(lod.name.size());
			writer.WriteString(lod.name);
			writer.WriteUInt32(lod.meshes.size());
			for (auto& mesh : lod.meshes)
			{
				writer.WriteUInt32(mesh.name.size());
				writer.WriteString(mesh.name);
				writer.WriteUInt32(mesh.elementMask);
				writer.WriteUInt32(mesh.lodId);
				writer.WriteUInt32(GetPackedVertexSize((elements::ELEMENT_TYPE)mesh.elementMask));
				writer.WriteUInt32(mesh.processedVertices.size());
				const uint32_t indexSize = (mesh.processedVertices.size() < (1 << 16)) ? sizeof(uint16_t) : sizeof(uint32_t);
				writer.WriteUInt32(indexSize);
				writer.WriteUInt32(mesh.indices.size());
				writer.WriteFloat(mesh.lodThreshold);
				PackVertices(mesh, writer);
				if (indexSize == sizeof(uint16_t))
				{
					for (auto it = mesh.indices.begin(); it < mesh.indices.end(); it++)
					{
						writer.WriteUInt16((uint16_t)*it);
					}
				}
				else
				{
					for (auto it = mesh.indices.begin(); it < mesh.indices.end(); it++)
					{
						writer.WriteUInt32((uint32_t)*it);
					}
				}
			}
		}
	}

	void ProcessScene(SCENE& scene, const IMPORT_PARAMS& params)
	{
		for (auto& lod : scene.lod)
		{
			for (auto& mesh : lod.meshes)
			{
				ProcessVertices(mesh, params);
			}
		}
	}

	void CreatePrimitive(PRIMITIVE_DESC& desc, IMPORT_PARAMS& params, GEOMETRY_DATA& data)
	{
		assert(desc.type < PRIMITIVE_TYPE::Count);
		SCENE scene;
		//g_primitiveCreators[(size_t)desc.type](scene, desc);
		GeneratePrimitive(scene, desc);
		ProcessScene(scene, params);
		PackData(scene, data);
	}
}