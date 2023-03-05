#pragma once

#ifdef _CMANAGER
#define CMANAGER_API extern "C" __declspec(dllexport)
#else
#define CMANAGER_API extern "C" __declspec(dllimport)
#endif