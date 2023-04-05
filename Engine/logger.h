#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace engine
{
	extern std::shared_ptr<spdlog::logger> logger;
	void LogInit(HANDLE hLog);
	void LogShutdown();
}


#ifdef _DEBUG
#define LOG_TRACE(...) logger->log(spdlog::level::trace, __VA_ARGS__)
#else
#define LOG_TRACE(...)
#endif
#define LOG_WARN(...) logger->log(spdlog::level::warn, __VA_ARGS__)
#define LOG_CRITICAL(...) logger->log(spdlog::level::critical, __VA_ARGS__)
#define LOG_ERROR(...) logger->log(spdlog::level::err, __VA_ARGS__)
#define LOG_DEBUG(...) logger->log(spdlog::level::debug, __VA_ARGS__)
#define LOG_INFO(...) logger->log(spdlog::level::info, __VA_ARGS__)