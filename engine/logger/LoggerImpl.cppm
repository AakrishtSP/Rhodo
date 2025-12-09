module;
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

export module Rhodo.Logger:Impl;
import :Structures;

namespace rhodo::logger::impl {
inline static bool g_backend_initialized = false;

inline void EnsureBackendStarted() {
  if (!g_backend_initialized) [[unlikely]] {
    g_backend_initialized = true;
    quill::Backend::start();
    std::atexit([]() { quill::Backend::stop(); });
  }
}

inline auto ToQuillLevel(const LogLevel kLevel) -> quill::LogLevel {
  switch (kLevel) {
    case LogLevel::TraceL3:
      return quill::LogLevel::TraceL3;
    case LogLevel::TraceL2:
      return quill::LogLevel::TraceL2;
    case LogLevel::TraceL1:
      return quill::LogLevel::TraceL1;
    case LogLevel::Debug:
      return quill::LogLevel::Debug;
    case LogLevel::Info:
      return quill::LogLevel::Info;
    case LogLevel::Notice:
      return quill::LogLevel::Notice;
    case LogLevel::Warning:
      return quill::LogLevel::Warning;
    case LogLevel::Error:
      return quill::LogLevel::Error;
    case LogLevel::Critical:
      return quill::LogLevel::Critical;
    case LogLevel::Backtrace:
      return quill::LogLevel::Backtrace;
  }
  return quill::LogLevel::Info;
}

class LoggerImpl {
 public:
  LoggerImpl() = delete;

  ~LoggerImpl() = default;

  explicit LoggerImpl(const LoggerConfig& config);

  auto Log(LogLevel level, const char* fmt, auto&&... args) -> void;

  auto SetLevel(LogLevel level) const -> void;

  auto Flush() const -> void;

 private:
  quill::Logger* logger_ = nullptr;
};

LoggerImpl::LoggerImpl(const LoggerConfig& config) {
  EnsureBackendStarted();

  std::vector<std::shared_ptr<quill::Sink>> sinks;

  for (const auto& [type, identifier] : config.sinks) {
    if (type == SinkType::Console) {
      sinks.push_back(quill::Frontend::create_or_get_sink<quill::ConsoleSink>(identifier));
    } else if (type == SinkType::File) {
      sinks.push_back(quill::Frontend::create_or_get_sink<quill::FileSink>(identifier));
    }
  }

  const quill::PatternFormatterOptions kFormat{config.message_pattern, config.time_format,
                                               quill::Timezone::GmtTime};

  logger_ = quill::Frontend::create_or_get_logger(config.name, std::move(sinks), kFormat);
  logger_->set_log_level(ToQuillLevel(config.default_level));
}

auto LoggerImpl::Log(const LogLevel level, const char* fmt, auto&&... args) -> void {
  if (logger_) [[likely]] {
    QUILL_LOG_DYNAMIC(logger_, ToQuillLevel(level), fmt, std::forward<decltype(args)>(args)...);
  }
}

auto LoggerImpl::SetLevel(const LogLevel level) const -> void {
  if (logger_) {
    logger_->set_log_level(ToQuillLevel(level));
  }
}

auto LoggerImpl::Flush() const -> void {
  if (logger_) [[likely]] {
    logger_->flush_log();
  }
}
}   // namespace rhodo::logger::impl
