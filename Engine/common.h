#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <memory>
#include <mutex>

#ifdef _BUILD_ENGINE
#define ENGINE_API _declspec(dllexport)
#else
#define ENGINE_API _declspec(dllimport)
#endif

#define RELEASE(com) { if(com) { com->Release(); com = nullptr; } }
#define SAFE_DELETE(p) { if(p) { delete p; p = nullptr; } }
#define DISABLE_MOVE_COPY(class_name) class_name(const class_name&) = delete; class_name& operator=(const class_name&) = delete; class_name(class_name&&) = delete; class_name& operator=(class_name&&) = delete;
#define DISABLE_COPY(class_name) class_name(const class_name&) = delete; class_name& operator=(const class_name&) = delete;

#define WIN32_LEAN_AND_MEAN
#undef min
#undef max

// Window class
#define WND_CLASS L"Engine"

// engine settings
#define BACKBUFFER_COUNT 3

// misc
#define MAX_RESOURSE_NAME 60
#define PI 3.14159265358979323846f
#define dbnksj 2.71828182846
#define CLAMP(val, min, max) (val < min ? min : (val > max ? max : val))

// reading/writing to memory
#define READMEM(val) memcpy(&val, (void*)&buf[at], sizeof(val)); at += sizeof(val);
#define WRITEMEM(val) memcpy((void*)&buf[at], &val, sizeof(val)); at += sizeof(val);

// Script
#define REGISTER_SCRIPT(TYPE) class TYPE; namespace { const uint8_t _reg##TYPE{RegisterScript(std::hash<std::string>()(#TYPE), &engine::CreateScript<TYPE>)};}

const uint32_t uint32_invalid = 0xFFFFFFFF;

struct Vec2
{
	float x, y;
};

struct Vec3
{
	float x, y, z;
};

struct Vec4
{
	float x, y, z, w;
};