#include "logger.h"
#include <iostream>

HANDLE g_logHandle = {};

struct DEFAULT_OUTPUT
{
    static FILE* stream()
    {
        return stderr;
    }
    static HANDLE handle()
    {
        return g_logHandle;
    }
};

namespace engine
{
    std::shared_ptr<spdlog::logger> logger;
    void LogInit(HANDLE hLog)
    {
        g_logHandle = hLog;
        if(g_logHandle)
            logger = spdlog::stderr_color_mt<DEFAULT_OUTPUT, spdlog::default_factory>("Engine");
        else
            logger = spdlog::stderr_color_mt<spdlog::details::console_stderr, spdlog::default_factory>("Engine");
        spdlog::set_pattern("[%H:%M:%S] [%^%l%$] [thread %t] %v");
#ifdef _DEBUG
        spdlog::set_level(spdlog::level::trace);
#else
        spdlog::set_level(spdlog::level::info);
#endif
    }

    void LogShutdown()
    {
        logger.reset();
        spdlog::shutdown();
    }
}