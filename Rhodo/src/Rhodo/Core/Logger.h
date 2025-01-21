#pragma once

#include <memory>

#include "Rhodo/Core/Core.h"
#include "spdlog/spdlog.h"


namespace Rhodo
{
	class RHODO_API Logger
	{
	public:
		Logger();
		~Logger();

		static void Init();
		static void Shutdown();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger;};
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// CoreLogger macros
#define RH_CORE_TRACE(...)    ::Rhodo::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define RH_CORE_INFO(...)     ::Rhodo::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define RH_CORE_WARN(...)     ::Rhodo::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define RH_CORE_ERROR(...)    ::Rhodo::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define RH_CORE_CRITICAL(...) ::Rhodo::Logger::GetCoreLogger()->critical(__VA_ARGS__)

// ClientLogger macros
#define RH_TRACE(...)         ::Rhodo::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define RH_INFO(...)          ::Rhodo::Logger::GetClientLogger()->info(__VA_ARGS__)
#define RH_WARN(...)          ::Rhodo::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define RH_ERROR(...)         ::Rhodo::Logger::GetClientLogger()->error(__VA_ARGS__)
#define RH_CRITICAL(...)      ::Rhodo::Logger::GetClientLogger()->critical(__VA_ARGS__)
