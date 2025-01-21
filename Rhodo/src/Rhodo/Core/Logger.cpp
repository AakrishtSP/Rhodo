#include "Logger.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Rhodo
{
	std::shared_ptr<spdlog::logger> Logger::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Logger::s_ClientLogger;

	Logger::Logger()
	= default;

	Logger::~Logger()
	= default;

	void Logger::Init()
	{
		s_CoreLogger = spdlog::create<spdlog::sinks::stdout_color_sink_mt>("RHODO");
		s_CoreLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] %^[%n: %l] [%t] [%s:%#] %v%$");
		s_CoreLogger->set_level(spdlog::level::trace);
		s_ClientLogger = spdlog::create<spdlog::sinks::stdout_color_sink_mt>("APP");
		s_ClientLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] %^[%n: %l] [%t] [%s:%#] %v%$");
		s_ClientLogger->set_level(spdlog::level::trace);
	}
	void Logger::Shutdown()
	{
	}
}