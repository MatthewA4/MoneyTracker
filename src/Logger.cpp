// Logger implementation using spdlog
#include "Logger.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace mt {
void Logger::init() {
    static bool initialized = false;
    if (initialized) return;
    auto logger = spdlog::stdout_color_mt("moneytracker");
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);
    initialized = true;
}

void Logger::info(const std::string& msg) { init(); spdlog::info("{}", msg); }
void Logger::warn(const std::string& msg) { init(); spdlog::warn("{}", msg); }
void Logger::error(const std::string& msg) { init(); spdlog::error("{}", msg); }
void Logger::debug(const std::string& msg) { init(); spdlog::debug("{}", msg); }
}
