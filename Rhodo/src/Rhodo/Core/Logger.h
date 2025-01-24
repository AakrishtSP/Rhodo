#pragma once

#include <memory>

#include "Rhodo/Core/Core.h"
#include "spdlog/spdlog.h"


namespace Rhodo {
	class RHODO_API Logger {
	public:
		Logger();

		~Logger();

		static void init();

		static void shutdown();

		inline static std::shared_ptr<spdlog::logger> &getCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger> &getClientLogger() { return s_ClientLogger; };

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// CoreLogger macros
#define RH_CORE_TRACE(...)    ::Rhodo::Logger::getCoreLogger()->trace(__VA_ARGS__)
#define RH_CORE_INFO(...)     ::Rhodo::Logger::getCoreLogger()->info(__VA_ARGS__)
#define RH_CORE_WARN(...)     ::Rhodo::Logger::getCoreLogger()->warn(__VA_ARGS__)
#define RH_CORE_ERROR(...)    ::Rhodo::Logger::getCoreLogger()->error(__VA_ARGS__)
#define RH_CORE_CRITICAL(...) ::Rhodo::Logger::getCoreLogger()->critical(__VA_ARGS__)

// ClientLogger macros
#define RH_TRACE(...)         ::Rhodo::Logger::getClientLogger()->trace(__VA_ARGS__)
#define RH_INFO(...)          ::Rhodo::Logger::getClientLogger()->info(__VA_ARGS__)
#define RH_WARN(...)          ::Rhodo::Logger::getClientLogger()->warn(__VA_ARGS__)
#define RH_ERROR(...)         ::Rhodo::Logger::getClientLogger()->error(__VA_ARGS__)
#define RH_CRITICAL(...)      ::Rhodo::Logger::getClientLogger()->critical(__VA_ARGS__)
