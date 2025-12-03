module;
#include "quill/Logger.h"
#include <string>
#include <utility>
#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/sinks/ConsoleSink.h"
#include "quill/sinks/FileSink.h"

#include "quill/std/Array.h"
#include "quill/std/Chrono.h"
#include "quill/std/Deque.h"
#include "quill/std/FilesystemPath.h"
#include "quill/std/ForwardList.h"
#include "quill/std/List.h"
#include "quill/std/Map.h"
#include "quill/std/Optional.h"
#include "quill/std/Pair.h"
#include "quill/std/Set.h"
#include "quill/std/Tuple.h"
#include "quill/std/UnorderedMap.h"
#include "quill/std/UnorderedSet.h"
#include "quill/std/Vector.h"
#include "quill/std/WideString.h"


export module Rhodo.Logger;

export namespace Rhodo::Logger {

    enum class log_level {
        trace_l3,
        trace_l2,
        trace_l1,
        debug,
        info,
        notice,
        warning,
        error,
        critical,
        backtrace
    };

    inline quill::Logger *coreLogger = nullptr;
    inline quill::Logger *appLogger = nullptr;

    auto init() -> void {
        [[maybe_unused]] static const bool initialized = []()-> bool {
            quill::Backend::start();
            std::atexit([]() { quill::Backend::stop(); });
            auto rhodoConsoleSink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>(
                    "core_console_sink"
                    );
            const auto coreFormat = quill::PatternFormatterOptions{"%(time) [%(thread_id)] "
                                                                   "%(log_level:<9) %(logger:<5) %(message)",
                                                                   "%H:%M:%S.%Qms", quill::Timezone::GmtTime};
            auto rhodoFileSink = quill::Frontend::create_or_get_sink<quill::FileSink>("RhodoCore.log");
            coreLogger = quill::Frontend::create_or_get_logger(
                    "Core", {std::move(rhodoFileSink), std::move(rhodoConsoleSink)}, coreFormat
                    );

            coreLogger->set_log_level(quill::LogLevel::Debug);

            auto appConsoleSink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>(
                    "app_console_sink"
                    );
            auto appFileSink = quill::Frontend::create_or_get_sink<quill::FileSink>("RhodoApp.log");
            const auto appFormat = quill::PatternFormatterOptions{"%(time) [%(thread_id)] "
                                                                   "%(log_level:<9) %(logger:<5) %(message)",
                                                                   "%H:%M:%S.%Qms", quill::Timezone::GmtTime};
            appLogger = quill::Frontend::create_or_get_logger(
                    "App", {std::move(appConsoleSink), std::move(appFileSink)}, appFormat
                    );
            appLogger->set_log_level(quill::LogLevel::Debug);
            return true;
        }();
    }

    auto flush() -> void {
        appLogger->flush_log();
        coreLogger->flush_log();
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
        QUILL_LOG_DYNAMIC(appLogger, quill::LogLevel::TraceL3, fmt, std::forward<decltype(args)>(args)...);
    }

    auto trace_l2(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
        QUILL_LOG_DYNAMIC(appLogger, quill::LogLevel::TraceL2, fmt, std::forward<decltype(args)>(args)...);
    }

    auto trace_l1(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
        QUILL_LOG_DYNAMIC(appLogger, quill::LogLevel::TraceL1, fmt, std::forward<decltype(args)>(args)...);
    }

    auto debug(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
        QUILL_LOG_DYNAMIC(appLogger, quill::LogLevel::Debug, fmt, std::forward<decltype(args)>(args)...);
    }

    auto info(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
        QUILL_LOG_DYNAMIC(appLogger, quill::LogLevel::Info, fmt, std::forward<decltype(args)>(args)...);
    }

    auto notice(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
        QUILL_LOG_DYNAMIC(appLogger, quill::LogLevel::Notice, fmt, std::forward<decltype(args)>(args)...);
    }

    auto warn(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
        QUILL_LOG_DYNAMIC(appLogger, quill::LogLevel::Warning, fmt, std::forward<decltype(args)>(args)...);
    }

    auto error(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
        QUILL_LOG_DYNAMIC(appLogger, quill::LogLevel::Error, fmt, std::forward<decltype(args)>(args)...);
    }

    auto critical(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
        QUILL_LOG_DYNAMIC(appLogger, quill::LogLevel::Critical, fmt, std::forward<decltype(args)>(args)...);
    }

    auto backtrace(const char *fmt, auto &&... args) -> void {
        if (appLogger) [[likely]]
        QUILL_LOG_DYNAMIC(appLogger, quill::LogLevel::Backtrace, fmt, std::forward<decltype(args)>(args)...);
    }

    auto trace_l3_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
        QUILL_LOG_DYNAMIC(coreLogger, quill::LogLevel::TraceL3, fmt, std::forward<decltype(args)>(args)...);
    }

    auto trace_l2_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
        QUILL_LOG_DYNAMIC(coreLogger, quill::LogLevel::TraceL2, fmt, std::forward<decltype(args)>(args)...);
    }

    auto trace_l1_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
        QUILL_LOG_DYNAMIC(coreLogger, quill::LogLevel::TraceL1, fmt, std::forward<decltype(args)>(args)...);
    }

    auto debug_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
        QUILL_LOG_DYNAMIC(coreLogger, quill::LogLevel::Debug, fmt, std::forward<decltype(args)>(args)...);
    }

    auto info_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
        QUILL_LOG_DYNAMIC(coreLogger, quill::LogLevel::Info, fmt, std::forward<decltype(args)>(args)...);
    }

    auto notice_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
        QUILL_LOG_DYNAMIC(coreLogger, quill::LogLevel::Notice, fmt, std::forward<decltype(args)>(args)...);
    }

    auto warn_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
        QUILL_LOG_DYNAMIC(coreLogger, quill::LogLevel::Warning, fmt, std::forward<decltype(args)>(args)...);
    }

    auto error_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
        QUILL_LOG_DYNAMIC(coreLogger, quill::LogLevel::Error, fmt, std::forward<decltype(args)>(args)...);
    }

    auto critical_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
        QUILL_LOG_DYNAMIC(coreLogger, quill::LogLevel::Critical, fmt, std::forward<decltype(args)>(args)...);
    }

    auto backtrace_core(const char *fmt, auto &&... args) -> void {
        if (coreLogger) [[likely]]
        QUILL_LOG_DYNAMIC(coreLogger, quill::LogLevel::Backtrace, fmt, std::forward<decltype(args)>(args)...);
    }

    // -------------------- Value logging --------------------
    auto vdebug(const char *msg, auto &&... vars) -> void {
        if (appLogger) [[likely]]
        QUILL_LOGV_DYNAMIC(appLogger, quill::LogLevel::Debug, msg);
    }

    auto vinfo(const char *msg, auto &&... vars) -> void {
        if (appLogger) [[likely]]
        QUILL_LOGV_DYNAMIC(appLogger, quill::LogLevel::Info, msg);
    }

    auto vwarn(const char *msg, auto &&... vars) -> void {
        if (appLogger) [[likely]]
        QUILL_LOGV_DYNAMIC(appLogger, quill::LogLevel::Warning, msg);
    }

    auto verror(const char *msg, auto &&... vars) -> void {
        if (appLogger) [[likely]]
        QUILL_LOGV_DYNAMIC(appLogger, quill::LogLevel::Error, msg);
    }

    auto vdebug_core(const char *msg, auto &&... args) -> void {
        if (coreLogger) [[likely]]
        QUILL_LOGV_DYNAMIC(coreLogger, quill::LogLevel::Debug, msg);
    }

    auto vinfo_core(const char *msg, auto &&... vars) -> void {
        if (coreLogger) [[likely]]
        QUILL_LOGV_DYNAMIC(coreLogger, quill::LogLevel::Info, msg);
    }

    auto vwarn_core(const char *msg, auto &&... vars) -> void {
        if (coreLogger) [[likely]]
        QUILL_LOGV_DYNAMIC(coreLogger, quill::LogLevel::Warning, msg);
    }

    auto verror_core(const char *msg, auto &&... vars) -> void {
        if (coreLogger) [[likely]]
        QUILL_LOGV_DYNAMIC(coreLogger, quill::LogLevel::Error, msg);
    }
}
