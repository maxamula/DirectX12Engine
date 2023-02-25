#include "shaders.h"
#include "Dependencies/nameof.hpp"

#include <iostream>

namespace engine
{
	extern HINSTANCE g_hInstance;
	namespace shaders
	{
		namespace
		{
			IDxcBlob* g_engineShaders[ENGINE_SHADER::NUM_SHADERS];

			SHADER_TYPE::type GetShaderTypeFromName(const wchar_t* name)
			{
				if (wcsncmp(name, L"VS_", 3) == 0)
					return SHADER_TYPE::VERTEX;
				else if (wcsncmp(name, L"HS_", 3) == 0)
					return SHADER_TYPE::HULL;
				else if (wcsncmp(name, L"DS_", 3) == 0)
					return SHADER_TYPE::DOMAIN;
				else if (wcsncmp(name, L"GS_", 3) == 0)
					return SHADER_TYPE::GEOMETRY;
				else if (wcsncmp(name, L"PS_", 3) == 0)
					return SHADER_TYPE::PIXEL;
				else if (wcsncmp(name, L"CS_", 3) == 0)
					return SHADER_TYPE::COMPUTE;
				else if (wcsncmp(name, L"AS_", 3) == 0)
					return SHADER_TYPE::AMPLIFICATION;
				else if (wcsncmp(name, L"MS_", 3) == 0)
					return SHADER_TYPE::MESH;
				else
					return SHADER_TYPE::NUM_SHADER_TYPES;
			}
		}

		ShaderCompiler::ShaderCompiler()
		{
			assert(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_compiler)) == S_OK);
			assert(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_utils)) == S_OK);
			assert(m_utils->CreateDefaultIncludeHandler(&m_includeHandler) == S_OK);
		}

		ShaderCompiler::~ShaderCompiler()
		{
			RELEASE(m_compiler);
			RELEASE(m_utils);
			RELEASE(m_includeHandler);
		}

		IDxcBlob* ShaderCompiler::CompileShader(const void* pShaderSource, uint32_t size, SHADER_TYPE::type shaderType)
		{
			IDxcBlobEncoding* shaderSource = nullptr;
			assert(m_utils->CreateBlobFromPinned(pShaderSource, size, CP_UTF8, &shaderSource) == S_OK);
			assert(shaderSource);
			//create dxcbuffer
			DxcBuffer buffer;
			buffer.Encoding = CP_UTF8;
			buffer.Ptr = shaderSource->GetBufferPointer();
			buffer.Size = shaderSource->GetBufferSize();

			// compiler args
			LPCWSTR args[]
			{
				L"-E", L"main",
				L"-T", m_profiles[shaderType],
				DXC_ARG_ALL_RESOURCES_BOUND,
#if _DEBUG
				DXC_ARG_DEBUG,
				DXC_ARG_SKIP_OPTIMIZATIONS,
#else
				DXC_ARG_OPTIMIZATION_LEVEL3,
#endif
				L"-Qstrip_reflect",
				L"-Qstrip_debug"
			};
			IDxcResult* compileResult = nullptr;
			assert(m_compiler->Compile(&buffer, args, std::size(args), m_includeHandler, IID_PPV_ARGS(&compileResult)) == S_OK);
			assert(compileResult);
			IDxcBlob* shader = nullptr;
			assert(compileResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shader), nullptr) == S_OK);

			// Release interfaces
			RELEASE(shaderSource);
			RELEASE(compileResult);
			return shader;
		}	

		IDxcBlob* GetEngineShader(ENGINE_SHADER::id id)
		{
			assert(id < ENGINE_SHADER::NUM_SHADERS);
			IDxcBlob* shader = g_engineShaders[id];
			assert(shader && shader->GetBufferSize());
			return shader;
		}

		void Initialize()
		{
			// loop through all dll resources
			for (uint32_t shaderIndex = 0; shaderIndex < ENGINE_SHADER::NUM_SHADERS; shaderIndex++)
			{
				std::string_view view = nameof::nameof_enum<ENGINE_SHADER::id>(static_cast<ENGINE_SHADER::id>(0));
				wchar_t resourceName[MAX_RESOURSE_NAME];
				MultiByteToWideChar(CP_UTF8, 0, view.data(), view.size(), resourceName, MAX_RESOURSE_NAME);
				resourceName[view.size()] = L'\0';
				HRSRC resource = FindResource(engine::g_hInstance, resourceName, L"ENGINE_SHADER");
				// load resource in memory
				assert(resource);
				HGLOBAL resourceData = LoadResource(engine::g_hInstance, resource);
				assert(resourceData);
				const void* data = LockResource(resourceData);
				// get resource size
				uint32_t size = SizeofResource(engine::g_hInstance, resource);
				
				ShaderCompiler compiler{};
				//compile shader
				auto blob = compiler.CompileShader(data, size, GetShaderTypeFromName(resourceName));
				// save shader
				g_engineShaders[shaderIndex] = blob;
				// release resource memory and free resource
				UnlockResource(resourceData);
				FreeResource(resourceData);
			}
		}

		void Shutdown()
		{
			for (uint32_t shaderIndex = 0; shaderIndex < ENGINE_SHADER::NUM_SHADERS; shaderIndex++)
				RELEASE(g_engineShaders[shaderIndex]);
		}
	}
}