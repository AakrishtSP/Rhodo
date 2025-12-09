module;
#include <bits/move.h>
export module Rhodo.Logger;

export import :Structures;
import :Impl;

export namespace rhodo::logger {

template <typename Tag>
class Logger {
 public:
  Logger() = default;

#define RHODO_LOG_LEVEL(name)                                            \
  template <typename... Args>                                            \
  static auto name(const char* fmt, Args&&... args) -> void {            \
    getInstance().Log(LogLevel::name, fmt, std::forward<Args>(args)...); \
  }
  RHODO_LOG_LEVEL(TraceL1)
  RHODO_LOG_LEVEL(TraceL2)
  RHODO_LOG_LEVEL(TraceL3)
  static auto Trace(const char* fmt, auto&&... args) -> void {
    getInstance().Log(LogLevel::TraceL1, fmt, std::forward<decltype(args)>(args)...);
  }
  RHODO_LOG_LEVEL(Debug)
  RHODO_LOG_LEVEL(Info)
  RHODO_LOG_LEVEL(Notice)
  RHODO_LOG_LEVEL(Warning)
  RHODO_LOG_LEVEL(Error)
  RHODO_LOG_LEVEL(Critical)
  RHODO_LOG_LEVEL(Backtrace)
#undef RHODO_LOG_LEVEL
  static auto SetLevel(const LogLevel kLevel) -> void { getInstance().SetLevel(kLevel); }
  static auto Flush() -> void { getInstance().Flush(); }

 private:
  static auto getInstance() -> impl::LoggerImpl& {
    static impl::LoggerImpl instance{Tag::Config()};
    return instance;
  }
};

// Logger tags with embedded configuration
struct CoreTag {
  static auto Config() -> LoggerConfig {
    return {
        .name         = "Core",
        .sinks         = {{SinkType::File, "Rhodo.log"}, {SinkType::Console, "console"}},
        .default_level = LogLevel::Debug,
    };
  }
};

struct AppTag {
  static auto Config() -> LoggerConfig {
    return {.name         = "App",
            .sinks         = {{SinkType::File, "Rhodo.log"}, {SinkType::Console, "console"}},
            .default_level = LogLevel::Debug};
  }
};

// Convenient type aliases
using CoreLogger = Logger<CoreTag>;
using AppLogger  = Logger<AppTag>;

}   // namespace rhodo::logger
