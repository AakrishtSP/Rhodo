module;
#include <cstdint>
#include <string>
#include <vector>

export module Rhodo.Logger:Structures;

export namespace rhodo::logger {
enum class LogLevel : std::uint8_t {
  TraceL3,
  TraceL2,
  TraceL1,
  Debug,
  Info,
  Notice,
  Warning,
  Error,
  Critical,
  Backtrace
};

enum class SinkType : std::uint8_t {
  Console,
  File
};

// Configuration structures
struct SinkConfig {
  SinkType    type;
  std::string identifier;
};

struct LoggerConfig {
  std::string             name;
  std::vector<SinkConfig> sinks;
  std::string             time_format = "%H:%M:%S.%Qms";
  std::string message_pattern = "%(time) [%(thread_id)] %(log_level:<9) %(logger:<12) %(message)";
  LogLevel    default_level   = LogLevel::Debug;
};
}   // namespace rhodo::logger
