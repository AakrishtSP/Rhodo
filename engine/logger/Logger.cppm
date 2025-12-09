module;

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
  static void name(const char* fmt, Args&&... args) {                    \
    getInstance().log(logLevel::name, fmt, std::forward<Args>(args)...); \
  }
  RHODO_LOG_LEVEL(trace_l3)
  RHODO_LOG_LEVEL(trace_l2)
  RHODO_LOG_LEVEL(trace_l1)
  static void trace(const char* fmt, auto&&... args) {
    getInstance().log(logLevel::trace_l1, fmt, std::forward<decltype(args)>(args)...);
  }
  RHODO_LOG_LEVEL(debug)
  RHODO_LOG_LEVEL(info)
  RHODO_LOG_LEVEL(notice)
  RHODO_LOG_LEVEL(warning)
  RHODO_LOG_LEVEL(error)
  RHODO_LOG_LEVEL(critical)
  RHODO_LOG_LEVEL(backtrace)
#undef RHODO_LOG_LEVEL
  static void setLevel(const logLevel level) { getInstance().setLevel(level); }
  static void flush() { getInstance().flush(); }

 private:
  static Impl::LoggerImpl& getInstance() {
    static Impl::LoggerImpl instance{Tag::config()};
    return instance;
  }
};

// Logger tags with embedded configuration
struct CoreTag {
  static LoggerConfig Config() {
    return {
        .name         = "Core",
        .sinks        = {{sinkType::file, "Rhodo.log"}, {sinkType::console, "console"}},
        .defaultLevel = logLevel::debug,
    };
  }
};

struct AppTag {
  static LoggerConfig Config() {
    return {.name         = "App",
            .sinks        = {{sinkType::file, "Rhodo.log"}, {sinkType::console, "console"}},
            .defaultLevel = logLevel::debug};
  }
};

// Convenient type aliases
using CoreLogger = Logger<CoreTag>;
using AppLogger  = Logger<AppTag>;

}   // namespace rhodo::logger
