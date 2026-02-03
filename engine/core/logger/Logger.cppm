module;
#include <utility>

export module Rhodo.Core.Logger;

export import :Structures;
import :Impl;

export namespace rhodo::core::logger {
    template<Tag T>
    class Logger {
    public:
        Logger() = default;

        template<LogLevel Level, typename... Args>
        static auto log(const char *fmt, Args &&... args) -> void {
            get_instance().log(Level, fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static auto trace_l1(const char *fmt, Args &&... args) -> void {
            log<LogLevel::TraceL1>(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static auto trace_l2(const char *fmt, Args &&... args) -> void {
            log<LogLevel::TraceL2>(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static auto trace_l3(const char *fmt, Args &&... args) -> void {
            log<LogLevel::TraceL3>(fmt, std::forward<Args>(args)...);
        }

        static auto trace(const char *fmt, auto &&... args) -> void {
            TraceL1(fmt, std::forward<decltype(args)>(args)...);
        }

        template<typename... Args>
        static auto debug(const char *fmt, Args &&... args) -> void {
            log<LogLevel::Debug>(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static auto info(const char *fmt, Args &&... args) -> void {
            log<LogLevel::Info>(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static auto notice(const char *fmt, Args &&... args) -> void {
            log<LogLevel::Notice>(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static auto warning(const char *fmt, Args &&... args) -> void {
            log<LogLevel::Warning>(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static auto error(const char *fmt, Args &&... args) -> void {
            log<LogLevel::Error>(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static auto critical(const char *fmt, Args &&... args) -> void {
            log<LogLevel::Critical>(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static auto backtrace(const char *fmt, Args &&... args) -> void {
            log<LogLevel::Backtrace>(fmt, std::forward<Args>(args)...);
        }

        static auto set_level(const LogLevel level) -> void {
            get_instance().set_level(level);
        }

        static auto flush() -> void { get_instance().flush(); }

    private:
        static auto get_instance() -> impl::LoggerImpl & {
            static impl::LoggerImpl instance{T::config()};
            return instance;
        }
    };

    // Logger tags with embedded configuration
    struct CoreTag {
        static auto config() -> LoggerConfig {
            return {
                .name = "Core",
                .sinks = {
                    {SinkType::File, "Rhodo.Core.Log"},
                    {SinkType::Console, "console"}
                },
                .default_level = LogLevel::Debug,
            };
        }
    };

    struct AppTag {
        static auto config() -> LoggerConfig {
            return {
                .name = "App",
                .sinks = {
                    {SinkType::File, "Rhodo.Core.Log"},
                    {SinkType::Console, "console"}
                },
                .default_level = LogLevel::Debug
            };
        }
    };

    // Convenient type aliases
    using CoreLogger = Logger<CoreTag>;
    using AppLogger = Logger<AppTag>;
} // namespace rhodo::core::logger
