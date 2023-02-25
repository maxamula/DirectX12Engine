#pragma once
#include "graphics.h"
#include "resource.h"
// include directx compiling lib
#include <dxcapi.h>
// link dxc compiler library
#pragma comment(lib, "dxcompiler.lib")
#undef DOMAIN

namespace engine::shaders
{
	struct SHADER_TYPE
	{
		enum type : uint32_t
		{
			VERTEX,
			HULL,
			DOMAIN,	
			GEOMETRY,
			PIXEL,
			COMPUTE,
			AMPLIFICATION,
			MESH,
			NUM_SHADER_TYPES
		};
	};

	struct ENGINE_SHADER
	{
		enum id : uint32_t
		{
			VS_FULLSCREEN,
			NUM_SHADERS
		};
	};

	class ShaderCompiler
	{
	public:
		ShaderCompiler();
		~ShaderCompiler();
		DISABLE_MOVE_COPY(ShaderCompiler);
		IDxcBlob* CompileShader(const void* pShaderSource, uint32_t size, SHADER_TYPE::type shaderType);
	private:
		const wchar_t* m_profiles[SHADER_TYPE::NUM_SHADER_TYPES]{ L"vs_6_5", L"hs_6_5", L"ds_6_5", L"gs_6_5", L"ps_6_5", L"cs_6_5", L"as_6_5", L"ms_6_5" };

		IDxcCompiler3* m_compiler;
		IDxcUtils* m_utils;	
		IDxcIncludeHandler* m_includeHandler;
	};

	IDxcBlob* GetEngineShader(ENGINE_SHADER::id id);
	void Initialize();
	void Shutdown();
}

