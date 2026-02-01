module;
#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/sinks/ConsoleSink.h"
#include "quill/sinks/FileSink.h"
// NOLINTBEGIN(*-include-cleaner)
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
// NOLINTEND(*-include-cleaner)

export module Rhodo.Core.Logger:Impl;
import :Structures;

namespace rhodo::core::logger::impl {
static bool g_backend_initialized = false;

static auto EnsureBackendStarted() -> void {
  if (!g_backend_initialized) [[unlikely]] {
    g_backend_initialized = true;
    quill::Backend::start();
    static_cast<void>(std::atexit([]() -> void { quill::Backend::stop(); }));
  }
}

static auto ToQuillLevel(const LogLevel level) -> quill::LogLevel {
  switch (level) {
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
  LoggerImpl(const LoggerImpl&) = delete;
  LoggerImpl(LoggerImpl&&) = delete;
  auto operator=(const LoggerImpl&) -> LoggerImpl& = delete;
  auto operator=(LoggerImpl&&) -> LoggerImpl& = delete;

  ~LoggerImpl() = default;

  explicit LoggerImpl(const LoggerConfig& config);

  auto Log(LogLevel k_level, const char* fmt, auto&&... args) -> void;

  auto SetLevel(LogLevel k_level) const -> void;

  auto Flush() const -> void;

 private:
  quill::Logger* logger_ = nullptr;
};

LoggerImpl::LoggerImpl(const LoggerConfig& config) {
  EnsureBackendStarted();

  std::vector<std::shared_ptr<quill::Sink>> sinks;

  for (const auto& [type, identifier] : config.sinks) {
    if (type == SinkType::Console) {
      sinks.push_back(
          quill::Frontend::create_or_get_sink<quill::ConsoleSink>(identifier));
    } else if (type == SinkType::File) {
      sinks.push_back(
          quill::Frontend::create_or_get_sink<quill::FileSink>(identifier));
    }
  }

  const quill::PatternFormatterOptions format{
      config.message_pattern, config.time_format, quill::Timezone::GmtTime};

  logger_ = quill::Frontend::create_or_get_logger(config.name, std::move(sinks),
                                                  format);
  logger_->set_log_level(ToQuillLevel(config.default_level));
}

auto LoggerImpl::Log(const LogLevel k_level, const char* fmt, auto&&... args)
    -> void {
  if (logger_ != nullptr) [[likely]] {
    QUILL_LOG_DYNAMIC(logger_, ToQuillLevel(k_level), fmt,
                      std::forward<decltype(args)>(args)...);
  }
}

auto LoggerImpl::SetLevel(const LogLevel k_level) const -> void {
  if (logger_ != nullptr) {
    logger_->set_log_level(ToQuillLevel(k_level));
  }
}

auto LoggerImpl::Flush() const -> void {
  if (logger_ != nullptr) [[likely]] {
    logger_->flush_log();
  }
}
}  // namespace rhodo::core::logger::impl
