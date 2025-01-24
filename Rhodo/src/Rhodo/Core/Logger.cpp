#include "Rhodo/Core/Logger.h"

#include "rhpch.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Rhodo {
	std::shared_ptr<spdlog::logger> Logger::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Logger::s_ClientLogger;

	Logger::Logger()
	= default;

	Logger::~Logger()
	= default;

	void Logger::init() {
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Rhodo.log", true));

		logSinks[0]->set_pattern("[%Y-%m-%d %H:%M:%S.%e] %^[%n: %l] [%t] %v%$");
		logSinks[1]->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n: %l] [%t] %v");

		s_CoreLogger = std::make_shared<spdlog::logger>("RHODO", begin(logSinks), end(logSinks));
		register_logger(s_CoreLogger);
		s_CoreLogger->set_level(spdlog::level::trace);
		s_CoreLogger->flush_on(spdlog::level::trace);
		s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
		register_logger(s_ClientLogger);
		s_ClientLogger->set_level(spdlog::level::trace);
		s_ClientLogger->flush_on(spdlog::level::trace);
	}

	void Logger::shutdown() {
	}
}
