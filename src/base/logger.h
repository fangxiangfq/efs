#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>
#include <string>

using loggerPtr = std::shared_ptr<spdlog::logger>;

class Logger
{
public:
    Logger(std::string&& filename)
    {
        filename_ = filename;
        logger_ = spdlog::create<spdlog::sinks::basic_file_sink_mt>("logger", filename_);
        stdout_ = spdlog::stdout_color_mt("stdout");
        
		spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%l] [%t] - <%s>|<%#>|<%!>,%v");
    }

    Logger(std::string&& filename)
    {
        logger_ = spdlog::create<spdlog::sinks::basic_file_sink_mt>("logger", filename_);
        stdout_ = spdlog::stdout_color_mt("stdout");
        
		spdlog::set_pattern("%Y-%m-%d %H:%M:%S [%l] [%t] - <%s>|<%#>|<%!>,%v");
    }
    
    ~Logger()=default;

private:
    loggerPtr stdout_;
	loggerPtr logger_;
	spdlog::filename_t filename_{"efs.log"};
};

#define STD_DEBUG(...) SPDLOG_LOGGER_DEBUG(spdlog::default_logger_raw(), __VA_ARGS__)
#define DEBUG(...) SPDLOG_LOGGER_DEBUG(spdlog::get("logger"), __VA_ARGS__)
#define STD_INFO(...) SPDLOG_LOGGER_INFO(spdlog::default_logger_raw(), __VA_ARGS__)
#define INFO(...) SPDLOG_LOGGER_INFO(spdlog::get("logger"), __VA_ARGS__)
#define STD_WARN(...) SPDLOG_LOGGER_WARN(spdlog::default_logger_raw(), __VA_ARGS__)
#define WARN(...) SPDLOG_LOGGER_WARN(spdlog::get("logger"), __VA_ARGS__)
#define STD_ERROR(...) SPDLOG_LOGGER_ERROR(spdlog::default_logger_raw(), __VA_ARGS__)
#define ERROR(...) SPDLOG_LOGGER_ERROR(spdlog::get("logger"), __VA_ARGS__)
#define STD_CRIT(...) SPDLOG_LOGGER_ERROR(spdlog::default_logger_raw(), __VA_ARGS__)
#define CRIT(...) SPDLOG_LOGGER_CRITICAL(spdlog::get("logger"), __VA_ARGS__)