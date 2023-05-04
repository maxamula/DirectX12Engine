#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <memory>
#include <mutex>
#include <assert.h>

// COMPILE SETTINGS
#define _DEBUG_GRAPHICS


#ifdef _BUILD_ENGINE
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#define RELEASE(com) { if(com) { com->Release(); com = nullptr; } }
#define SAFE_DELETE(p) { if(p) { delete p; p = nullptr; } }
#define DISABLE_MOVE_COPY(class_name) class_name(const class_name&) = delete; class_name& operator=(const class_name&) = delete; class_name(class_name&&) = delete; class_name& operator=(class_name&&) = delete;
#define DISABLE_COPY(class_name) class_name(const class_name&) = delete; class_name& operator=(const class_name&) = delete;
#define assert_throw(x, msg) if(!(x)) { throw std::exception(msg); }
#define DEVICE_CHECK assert_throw(device, "Device not initialized")

class com_exception : public std::exception
{
public:
    com_exception(HRESULT hr) : result(hr) {}

    const char* what() const noexcept override
    {
        static char s_str[64] = {};
        sprintf_s(s_str, "Failure with HRESULT of %08X",
            static_cast<unsigned int>(result));
        return s_str;
    }

private:
    HRESULT result;
};

// Helper utility converts D3D API failures into exceptions.
#define ThrowIfFailed(hr) if (FAILED(hr)) throw com_exception(hr)

#ifdef _DEBUG
#define SET_NAME(x, name) x->SetName(name)
#define INDEXED_NAME_BUFFER(size) wchar_t namebuf[size]
#define SET_NAME_INDEXED(x, name, index) swprintf_s(namebuf, L"%s (%d)", name, index); x->SetName(namebuf)
#else
#define SET_NAME(x, name) ((void)0)
#define INDEXED_NAME_BUFFER(size) ((void)0)
#define SET_NAME_INDEXED(x, name, index) ((void)0)
#endif

#define WIN32_LEAN_AND_MEAN
#undef min
#undef max

// Window class
#define WND_CLASS L"Engine"

// engine settings
#define BACKBUFFER_COUNT 3
#define COPY_FRAMES_COUNT 2
#define VSYNC_DEFAULT_STATE 1

// misc
#define MAX_RESOURSE_NAME 60
#define PI 3.14159265358979323846f
#define dbnksj 2.71828182846
#define CLAMP(val, min, max) (val < min ? min : (val > max ? max : val))

// reading/writing to memory
#define READMEM(val) memcpy(&val, (void*)&buf[at], sizeof(val)); at += sizeof(val);
#define WRITEMEM(val) memcpy((void*)&buf[at], &val, sizeof(val)); at += sizeof(val);

// Script
#ifndef _DEBUG
#define REGISTER_SCRIPT(TYPE) class TYPE; namespace { const uint8_t _reg##TYPE{RegisterScript(std::hash<std::string>()(#TYPE), &engine::CreateScript<TYPE>)};}
#else
#define REGISTER_SCRIPT(TYPE) class TYPE; namespace { const uint8_t _reg##TYPE{RegisterScript(std::hash<std::string>()(#TYPE), &engine::CreateScript<TYPE>, #TYPE)};}
#endif

const uint32_t uint32_invalid = 0xFFFFFFFF;

struct Vec2
{
	Vec2() = default;
	Vec2(float x, float y)
		: x(x), y(y)
	{}
	float x, y;
};

struct Vec3
{
	Vec3() = default;
	Vec3(float x, float y, float z)
		: x(x), y(y), z(z)
	{}
	float x, y, z;
};

struct Vec4
{
	Vec4() = default;
	Vec4(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w)
	{}
	float x, y, z, w;
};

struct UVec2
{
	uint32_t x, y;
};