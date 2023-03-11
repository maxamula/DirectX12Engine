#include "geometry.h"
#include "generator/PlaneMesh.hpp"
#include <DirectXMath.h>

#pragma comment(lib, "generator.lib")

using namespace DirectX;

namespace engine::content
{
	namespace
	{
		typedef void(*CreatePrimitiveFnPtr)(SCENE& scene, PRIMITIVE_DESC& desc);

		void CreatePlane(SCENE& scene, PRIMITIVE_DESC& desc);
		void CreateBox(SCENE& scene, PRIMITIVE_DESC& desc);
		void CreateSphere(SCENE& scene, PRIMITIVE_DESC& desc);

		CreatePrimitiveFnPtr g_primitiveCreators[] =
		{
			CreatePlane,
			CreateBox,
			CreateSphere
		};

		static_assert(sizeof(g_primitiveCreators) / sizeof(g_primitiveCreators[0]) == (size_t)PRIMITIVE_TYPE::Count);

		void CreatePlane(SCENE& scene, PRIMITIVE_DESC& desc)
		{
			LOD_GROUP lod;
			lod.name = "LOD0";
			MESH mesh;
			generator::PlaneMesh generated = generator::PlaneMesh({ (desc.size.x), desc.size.y }, { (int)desc.segments[0], (int)desc.segments[1] });
			auto vertices = generated.vertices();
			while (!vertices.done())
			{
				generator::MeshVertex vertex = vertices.generate();
				mesh.vertices.emplace_back(vertex.position[0], vertex.position[1], vertex.position[2]);
				//mesh.normals.emplace_back(vertex.normal[0], vertex.normal[1], vertex.normal[2]);	// TODO implement generation method
				//mesh.uvs[0].emplace_back(vertex.texCoord[0], vertex.texCoord[1]);
				vertices.next();
			}
			auto triangles = generated.triangles();
			while (!triangles.done())
			{
				generator::Triangle triangle = triangles.generate();
				mesh.indices.emplace_back(triangle.vertices[0]);
				mesh.indices.emplace_back(triangle.vertices[1]);
				mesh.indices.emplace_back(triangle.vertices[2]);
				triangles.next();
			}
			lod.meshes.push_back(mesh);
			scene.lod.push_back(lod);
		}

		void CreateBox(SCENE& scene, PRIMITIVE_DESC& desc)
		{

		}

		void CreateSphere(SCENE& scene, PRIMITIVE_DESC& desc)
		{

		}

		template <typename T>
		constexpr T PackFloat(float f)
		{
			assert(f >= 0.0f && f <= 1.0f);
			uint32_t bits = sizeof(T) * 8;
			float intervals = (float)((1ui32 << bits) - 1);
			return (T)(intervals * f + 0.5f);
		}

		template <typename T>
		constexpr T PackFloat(float f, float min, float max)
		{
			assert(min < max);
			uint32_t bits = sizeof(T) * 8;
			const float dist = (f - min) / (max - min);
			return PackFloat<T>(dist);
		}

		template <typename T>
		constexpr float UnpackFloat(T f)
		{
			uint32_t bits = sizeof(T) * 8;
			float intervals = (float)((1ui32 << bits) - 1);
			return (float)f / intervals;
		}

		template <typename T>
		constexpr float UnpackFloat(T f, float min, float max)
		{
			assert(min < max);
			return min + UnpackFloat(f) * (max - min);
		}

		void PackStaticVertices(MESH& mesh)
		{
			const uint32_t nVertices = (uint32_t)mesh.processedVertices.size();
			const uint32_t nIndices = (uint32_t)mesh.indices.size();
			mesh.pkStaticVertices.reserve(nVertices);
			for (uint32_t i = 0; i < nVertices; i++)
			{
				VERTEX& vertex = mesh.processedVertices[i];
				PK_STATIC_VERTEX pkVertex;

				pkVertex.position = vertex.position;
				const uint8_t signs = (uint8_t)(vertex.normal.z > 0.0f) << 1;
				const uint16_t nx = PackFloat<uint16_t>(vertex.normal.x);
				const uint16_t ny = PackFloat<uint16_t>(vertex.normal.y);
				// TODO tanfents

				mesh.pkStaticVertices.emplace_back(PK_STATIC_VERTEX
					{
						vertex.position,
						{0, 0, 0},
						signs,
						{nx, ny},
						{0, 0},
						vertex.uv
					});
			}
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
			const float cosAngle = XMScalarCos(PI - params.smoothingAngle * PI - 180.0f);
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
						XMStoreFloat(&cos, XMVector3Dot(normal1, normal2) * XMVector3ReciprocalLength(normal1));
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

			PackStaticVertices(mesh);
		}

		void ProcessUVs(MESH& mesh)
		{
			std::vector<VERTEX> oldVertices;
			oldVertices.swap(mesh.processedVertices);
			std::vector<uint32_t> oldIndices;
			oldIndices.swap(mesh.indices);

		}

		size_t GetMeshSize(const MESH& mesh)
		{
			const uint64_t nVertices = mesh.pkStaticVertices.size();
			const uint64_t vertexBufSize = nVertices * sizeof(PK_STATIC_VERTEX);
			const uint64_t indexSize = (nVertices < (1 << 16)) ? sizeof(uint16_t) : sizeof(uint32_t);
			const uint64_t indexBufferSize = mesh.indices.size() * indexSize;
			uint64_t size
			{
				sizeof(uint32_t) +
				mesh.name.size() +
				sizeof(uint32_t) +	// Mesh id
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

		size_t GetPackedBufferSize(const SCENE& scene)
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
					sizeof(uint32_t)
				};

				for (auto& mesh : lod.meshes)
				{
					lodSize += GetMeshSize(mesh);
				}
				size += lodSize;
			}
			return size;
		}

		void PackData(const SCENE& scene, GEOMETRY_DATA& geometry)
		{
			uint64_t bufSize = GetPackedBufferSize(scene);
			geometry.bufferSize = bufSize;
			geometry.buffer = CoTaskMemAlloc(bufSize);
			assert(geometry.buffer);
			uint8_t* buf = (uint8_t*)geometry.buffer;
			uint64_t at = 0;
			uint32_t u32temp;

			// Scene
			u32temp = (uint32_t)scene.name.size();
			WRITEMEM(u32temp);
			memcpy(&buf[at], scene.name.c_str(), u32temp); at += u32temp;
			u32temp = scene.lod.size();
			WRITEMEM(u32temp);

			for (auto& lod : scene.lod)
			{
				u32temp = (uint32_t)lod.name.size();
				WRITEMEM(u32temp);
				memcpy(&buf[at], lod.name.c_str(), u32temp); at += u32temp;
				u32temp = lod.meshes.size();
				WRITEMEM(u32temp);
				for (auto& mesh : lod.meshes)
				{
					u32temp = (uint32_t)mesh.name.size();
					WRITEMEM(u32temp);
					memcpy(&buf[at], mesh.name.c_str(), u32temp); at += u32temp;
					u32temp = mesh.lodId;
					WRITEMEM(u32temp);
					u32temp = sizeof(PK_STATIC_VERTEX);
					WRITEMEM(u32temp);
					u32temp = mesh.pkStaticVertices.size();
					WRITEMEM(u32temp);
					const uint32_t indexSize = (mesh.pkStaticVertices.size() < (1 << 16)) ? sizeof(uint16_t) : sizeof(uint32_t);
					u32temp = indexSize;
					WRITEMEM(u32temp);
					u32temp = mesh.indices.size();
					WRITEMEM(u32temp);

					//LOD
					float ftemp = mesh.lodThreshold;
					WRITEMEM(ftemp);
					u32temp = sizeof(PK_STATIC_VERTEX) * mesh.pkStaticVertices.size();
					memcpy(&buf[at], mesh.pkStaticVertices.data(), u32temp); at += u32temp;
					u32temp = indexSize * mesh.indices.size();
					void* data = (void*)mesh.indices.data();
					std::vector<uint16_t> indices16;
					if (indexSize == sizeof(uint16_t))
					{
						indices16.resize(mesh.indices.size());
						for (uint32_t i = 0; i < mesh.indices.size(); i++)
							indices16[i] = (uint16_t)mesh.indices[i];
						data = (void*)indices16.data();
					}
					memcpy(&buf[at], data, u32temp); at += u32temp;
				}
			}
		};

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
	}

	void CreatePrimitive(PRIMITIVE_DESC& desc, IMPORT_PARAMS& params, GEOMETRY_DATA& data)
	{
		assert(desc.type < PRIMITIVE_TYPE::Count);
		SCENE scene;
		g_primitiveCreators[(size_t)desc.type](scene, desc);
		ProcessScene(scene, params);
		PackData(scene, data);
	}
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{
	engine::content::IMPORT_PARAMS p{};
	p.bGenerateNormals = true;
	p.smoothingAngle = 60.0f;
	
	engine::content::PRIMITIVE_DESC d{};
	d.type = engine::content::PRIMITIVE_TYPE::Plane;
	d.segments[1] = 4;

	engine::content::GEOMETRY_DATA data{};

	engine::content::CreatePrimitive(d, p, data);
	return 0;
}