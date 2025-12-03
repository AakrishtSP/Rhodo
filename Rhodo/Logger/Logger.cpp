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
    void trace_l3(const char *fmt, auto &&... args) {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::TraceL3, fmt, std::forward<decltype(args)>(args)...);
    }

    void trace_l2(const char *fmt, auto &&... args) {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::TraceL2, fmt, std::forward<decltype(args)>(args)...);
    }

    void trace_l1(const char *fmt, auto &&... args) {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::TraceL1, fmt, std::forward<decltype(args)>(args)...);
    }

    void debug(const char *fmt, auto &&... args) {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::Debug, fmt, std::forward<decltype(args)>(args)...);
    }

    void info(const char *fmt, auto &&... args) {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::Info, fmt, std::forward<decltype(args)>(args)...);
    }

    void notice(const char *fmt, auto &&... args) {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::Notice, fmt, std::forward<decltype(args)>(args)...);
    }

    void warn(const char *fmt, auto &&... args) {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::Warning, fmt, std::forward<decltype(args)>(args)...);
    }

    void error(const char *fmt, auto &&... args) {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::Error, fmt, std::forward<decltype(args)>(args)...);
    }

    void critical(const char *fmt, auto &&... args) {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::Critical, fmt, std::forward<decltype(args)>(args)...);
    }

    void backtrace(const char *fmt, auto &&... args) {
        if (appLogger) [[likely]]
                QUILL_LOG(appLogger, quill::LogLevel::Backtrace, fmt, std::forward<decltype(args)>(args)...);
    }

    void trace_l3_core(const char *fmt, auto &&... args) {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::TraceL3, fmt, std::forward<decltype(args)>(args)...);
    }

    void trace_l2_core(const char *fmt, auto &&... args) {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::TraceL2, fmt, std::forward<decltype(args)>(args)...);
    }

    void trace_l1_core(const char *fmt, auto &&... args) {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::TraceL1, fmt, std::forward<decltype(args)>(args)...);
    }

    void debug_core(const char *fmt, auto &&... args) {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::Debug, fmt, std::forward<decltype(args)>(args)...);
    }

    void info_core(const char *fmt, auto &&... args) {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::Info, fmt, std::forward<decltype(args)>(args)...);
    }

    void notice_core(const char *fmt, auto &&... args) {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::Notice, fmt, std::forward<decltype(args)>(args)...);
    }

    void warn_core(const char *fmt, auto &&... args) {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::Warning, fmt, std::forward<decltype(args)>(args)...);
    }

    void error_core(const char *fmt, auto &&... args) {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::Error, fmt, std::forward<decltype(args)>(args)...);
    }

    void critical_core(const char *fmt, auto &&... args) {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::Critical, fmt, std::forward<decltype(args)>(args)...);
    }

    void backtrace_core(const char *fmt, auto &&... args) {
        if (coreLogger) [[likely]]
                QUILL_LOG(coreLogger, quill::LogLevel::Backtrace, fmt, std::forward<decltype(args)>(args)...);
    }

    // -------------------- Value logging --------------------
    void vdebug(const char *msg, auto &&... vars) {
        if (appLogger) [[likely]]
                QUILL_LOGV(appLogger, quill::LogLevel::Debug, msg, std::forward<decltype(vars)>(vars)...);
    }

    void vinfo(const char *msg, auto &&... vars) {
        if (appLogger) [[likely]]
                QUILL_LOGV(appLogger, quill::LogLevel::Info, msg, std::forward<decltype(vars)>(vars)...);
    }

    void vwarn(const char *msg, auto &&... vars) {
        if (appLogger) [[likely]]
                QUILL_LOGV(appLogger, quill::LogLevel::Warning, msg, std::forward<decltype(vars)>(vars)...);
    }

    void verror(const char *msg, auto &&... vars) {
        if (appLogger) [[likely]]
                QUILL_LOGV(appLogger, quill::LogLevel::Error, msg, std::forward<decltype(vars)>(vars)...);
    }

    void vdebug_core(const char *msg, auto &&... args) {
        if (coreLogger) [[likely]]
                QUILL_LOGV(coreLogger, quill::LogLevel::Debug, msg, std::forward<decltype(args)>(args)...);
    }

    void vinfo_core(const char *msg, auto &&... vars) {
        if (coreLogger) [[likely]]
                QUILL_LOGV(coreLogger, quill::LogLevel::Info, msg, std::forward<decltype(vars)>(vars)...);
    }

    void vwarn_core(const char *msg, auto &&... vars) {
        if (coreLogger) [[likely]]
                QUILL_LOGV(coreLogger, quill::LogLevel::Warning, msg, std::forward<decltype(vars)>(vars)...);
    }

    void verror_core(const char *msg, auto &&... vars) {
        if (coreLogger) [[likely]]
                QUILL_LOGV(coreLogger, quill::LogLevel::Error, msg, std::forward<decltype(vars)>(vars)...);
    }
}
