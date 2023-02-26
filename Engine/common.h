#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <memory>
#include <mutex>

#define ENGINE_API __declspec(dllexport)

#define RELEASE(com) { if(com) { com->Release(); com = nullptr; } }
#define SAFE_DELETE(p) { if(p) { delete p; p = nullptr; } }
#define DISABLE_MOVE_COPY(class_name) class_name(const class_name&) = delete; class_name& operator=(const class_name&) = delete; class_name(class_name&&) = delete; class_name& operator=(class_name&&) = delete;
#define DISABLE_COPY(class_name) class_name(const class_name&) = delete; class_name& operator=(const class_name&) = delete;

#define WIN32_LEAN_AND_MEAN

// Window class
#define WND_CLASS L"Engine"

// engine settings
#define BACKBUFFER_COUNT 3

// misc
#define MAX_RESOURSE_NAME 60

// reading/writing to memory
#define READMEM(val) memcpy(&val, (void*)at, sizeof(val)); at += sizeof(val);
#define WRITEMEM(val) memcpy((void*)at, &val, sizeof(val)); at += sizeof(val);

// Script
#define REGISTER_SCRIPT(TYPE) class TYPE; namespace { const uint8_t _reg##TYPE{RegisterScript(std::hash<std::string>()(#TYPE), &engine::CreateScript<TYPE>)};}