module;
#include <utility>

export module Rhodo.Core.Logger;

export import :Structures;
import :Impl;

export namespace rhodo::core::logger {

template <typename Tag>
class Logger {
 public:
  Logger() = default;
  template <LogLevel Level, typename... Args>
  static auto Log(const char* fmt, Args&&... args) -> void {
    GetInstance().Log(Level, fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static auto TraceL1(const char* fmt, Args&&... args) -> void {
    Log<LogLevel::TraceL1>(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static auto TraceL2(const char* fmt, Args&&... args) -> void {
    Log<LogLevel::TraceL2>(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static auto TraceL3(const char* fmt, Args&&... args) -> void {
    Log<LogLevel::TraceL3>(fmt, std::forward<Args>(args)...);
  }

  static auto Trace(const char* fmt, auto&&... args) -> void {
    TraceL1(fmt, std::forward<decltype(args)>(args)...);
  }

  template <typename... Args>
  static auto Debug(const char* fmt, Args&&... args) -> void {
    Log<LogLevel::Debug>(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static auto Info(const char* fmt, Args&&... args) -> void {
    Log<LogLevel::Info>(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static auto Notice(const char* fmt, Args&&... args) -> void {
    Log<LogLevel::Notice>(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static auto Warning(const char* fmt, Args&&... args) -> void {
    Log<LogLevel::Warning>(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static auto Error(const char* fmt, Args&&... args) -> void {
    Log<LogLevel::Error>(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static auto Critical(const char* fmt, Args&&... args) -> void {
    Log<LogLevel::Critical>(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static auto Backtrace(const char* fmt, Args&&... args) -> void {
    Log<LogLevel::Backtrace>(fmt, std::forward<Args>(args)...);
  }

  static auto SetLevel(const LogLevel level) -> void {
    GetInstance().SetLevel(level);
  }
  static auto Flush() -> void { GetInstance().Flush(); }

 private:
  static auto GetInstance() -> impl::LoggerImpl& {
    static impl::LoggerImpl instance{Tag::Config()};
    return instance;
  }
};

// Logger tags with embedded configuration
struct CoreTag {
  static auto Config() -> LoggerConfig {
    return {
        .name         = "Core",
        .sinks         = {{SinkType::File, "Rhodo.Core.Log"}, {SinkType::Console, "console"}},
        .default_level = LogLevel::Debug,
    };
  }
};

struct AppTag {
  static auto Config() -> LoggerConfig {
    return {.name         = "App",
            .sinks         = {{SinkType::File, "Rhodo.Core.Log"}, {SinkType::Console, "console"}},
            .default_level = LogLevel::Debug};
  }
};

// Convenient type aliases
using CoreLogger = Logger<CoreTag>;
using AppLogger  = Logger<AppTag>;

}   // namespace rhodo::core::logger
