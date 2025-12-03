module;
#include "quill/Logger.h"
#include <string>
#include <utility>
#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/sinks/ConsoleSink.h"
#include "quill/sinks/FileSink.h"

module Rhodo.Logger;

namespace Rhodo::Logger {
    inline quill::Logger *coreLogger = nullptr;
    inline quill::Logger *appLogger = nullptr;

    auto init() -> void {
        [[maybe_unused]] static const bool initialized = []()-> bool {
            quill::Backend::start();
            std::atexit([]() { quill::Backend::stop(); });
            auto rhodoConsoleSink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>(
                "core_console_sink"
            );
            auto rhodoFileSink = quill::Frontend::create_or_get_sink<quill::FileSink>("RhodoCore.log");
            coreLogger = quill::Frontend::create_or_get_logger(
                "RhodoCore", {std::move(rhodoConsoleSink), std::move(rhodoFileSink)}
            );
            coreLogger->set_log_level(quill::LogLevel::Debug);

            auto appConsoleSink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>(
                "app_console_sink"
            );
            auto appFileSink = quill::Frontend::create_or_get_sink<quill::FileSink>("RhodoApp.log");
            appLogger = quill::Frontend::create_or_get_logger(
                "RhodoApp", {std::move(appConsoleSink), std::move(appFileSink)}
            );
            appLogger->set_log_level(quill::LogLevel::Debug);
            return true;
        }();
    }

    auto to_quill(const log_level level) -> quill::LogLevel {
        switch (level) {
            case log_level::trace_l3: return quill::LogLevel::TraceL3;
            case log_level::trace_l2: return quill::LogLevel::TraceL2;
            case log_level::trace_l1: return quill::LogLevel::TraceL1;
            case log_level::debug: return quill::LogLevel::Debug;
            case log_level::info: return quill::LogLevel::Info;
            case log_level::notice: return quill::LogLevel::Notice;
            case log_level::warning: return quill::LogLevel::Warning;
            case log_level::error: return quill::LogLevel::Error;
            case log_level::critical: return quill::LogLevel::Critical;
            case log_level::backtrace: return quill::LogLevel::Backtrace;
        }
        return quill::LogLevel::Info;
    }

    auto set_core_level(log_level level) -> void {
        if (coreLogger)
            coreLogger->set_log_level(to_quill(level));
    }

    auto set_app_level(log_level level) -> void {
        if (appLogger)
            appLogger->set_log_level(to_quill(level));
    }

    // -------------------- Normal logging --------------------
    auto trace_l3(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::TraceL3, fmt, std::forward<decltype(args)>(args)...);
    }

    auto trace_l2(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::TraceL2, fmt, std::forward<decltype(args)>(args)...);
    }

    auto trace_l1(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::TraceL1, fmt, std::forward<decltype(args)>(args)...);
    }

    auto debug(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::Debug, fmt, std::forward<decltype(args)>(args)...);
    }

    auto info(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::Info, fmt, std::forward<decltype(args)>(args)...);
    }

    auto notice(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::Notice, fmt, std::forward<decltype(args)>(args)...);
    }

    auto warn(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::Warning, fmt, std::forward<decltype(args)>(args)...);
    }

    auto error(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::Error, fmt, std::forward<decltype(args)>(args)...);
    }

    auto critical(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::Critical, fmt, std::forward<decltype(args)>(args)...);
    }

    auto backtrace(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::Backtrace, fmt, std::forward<decltype(args)>(args)...);
    }

    auto trace_l3_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::TraceL3, fmt, std::forward<decltype(args)>(args)...);
    }

    auto trace_l2_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::TraceL2, fmt, std::forward<decltype(args)>(args)...);
    }

    auto trace_l1_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::TraceL1, fmt, std::forward<decltype(args)>(args)...);
    }

    auto debug_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::Debug, fmt, std::forward<decltype(args)>(args)...);
    }

    auto info_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::Info, fmt, std::forward<decltype(args)>(args)...);
    }

    auto notice_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::Notice, fmt, std::forward<decltype(args)>(args)...);
    }

    auto warn_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::Warning, fmt, std::forward<decltype(args)>(args)...);
    }

    auto error_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::Error, fmt, std::forward<decltype(args)>(args)...);
    }

    auto critical_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::Critical, fmt, std::forward<decltype(args)>(args)...);
    }

    auto backtrace_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::Backtrace, fmt, std::forward<decltype(args)>(args)...);
    }

    // -------------------- Value logging --------------------
    auto vdebug(const char *msg, auto &&... vars) -> void {
        if (appLogger) [[likely]]
                QUILL_LOGV(appLogger, quill::LogLevel::Debug, msg, std::forward<decltype(vars)>(vars)...);
    }

    auto vinfo(const char *msg, auto &&... vars) -> void {
        if (appLogger) [[likely]]
                QUILL_LOGV(appLogger, quill::LogLevel::Info, msg, std::forward<decltype(vars)>(vars)...);
    }

    auto vwarn(const char *msg, auto &&... vars) -> void {
        if (appLogger) [[likely]]
                QUILL_LOGV(appLogger, quill::LogLevel::Warning, msg, std::forward<decltype(vars)>(vars)...);
    }

    auto verror(const char *msg, auto &&... vars) -> void {
        if (appLogger) [[likely]]
                QUILL_LOGV(appLogger, quill::LogLevel::Error, msg, std::forward<decltype(vars)>(vars)...);
    }

    auto vdebug_core(const char *msg, auto &&... args) -> void {
        if (coreLogger) [[likely]]
                QUILL_LOGV(coreLogger, quill::LogLevel::Debug, msg, std::forward<decltype(args)>(args)...);
    }

    auto vinfo_core(const char *msg, auto &&... vars) -> void {
        if (coreLogger) [[likely]]
                QUILL_LOGV(coreLogger, quill::LogLevel::Info, msg, std::forward<decltype(vars)>(vars)...);
    }

    auto vwarn_core(const char *msg, auto &&... vars) -> void {
        if (coreLogger) [[likely]]
                QUILL_LOGV(coreLogger, quill::LogLevel::Warning, msg, std::forward<decltype(vars)>(vars)...);
    }

    auto verror_core(const char *msg, auto &&... vars) -> void {
        if (coreLogger) [[likely]]
                QUILL_LOGV(coreLogger, quill::LogLevel::Error, msg, std::forward<decltype(vars)>(vars)...);
    }
}
