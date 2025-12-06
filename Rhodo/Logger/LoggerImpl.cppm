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

namespace Rhodo::Logger::Impl {
    inline static bool backendInitialized = false;

    inline void ensureBackendStarted() {
        if (!backendInitialized) [[unlikely]] {
            backendInitialized = true;
            quill::Backend::start();
            std::atexit([]() { quill::Backend::stop(); });
        }
    }

    inline auto toQuillLevel(const logLevel level) -> quill::LogLevel {
        switch (level) {
            case logLevel::trace_l3: return quill::LogLevel::TraceL3;
            case logLevel::trace_l2: return quill::LogLevel::TraceL2;
            case logLevel::trace_l1: return quill::LogLevel::TraceL1;
            case logLevel::debug: return quill::LogLevel::Debug;
            case logLevel::info: return quill::LogLevel::Info;
            case logLevel::notice: return quill::LogLevel::Notice;
            case logLevel::warning: return quill::LogLevel::Warning;
            case logLevel::error: return quill::LogLevel::Error;
            case logLevel::critical: return quill::LogLevel::Critical;
            case logLevel::backtrace: return quill::LogLevel::Backtrace;
        }
        return quill::LogLevel::Info;
    }

    class LoggerImpl {
    public:
        LoggerImpl() = delete;

        ~LoggerImpl() = default;

        explicit LoggerImpl(const LoggerConfig &config);

        void log(logLevel level, const char *fmt, auto &&... args);

        void setLevel(logLevel level) const;

        void flush() const;

    private:
        quill::Logger *logger_ = nullptr;
    };

    LoggerImpl::LoggerImpl(const LoggerConfig &config) {
        ensureBackendStarted();

        std::vector<std::shared_ptr<quill::Sink> > sinks;

        for (const auto &[type, identifier]: config.sinks) {
            if (type == sinkType::console) {
                sinks.push_back(quill::Frontend::create_or_get_sink<quill::ConsoleSink>(
                                identifier
                                )
                        );
            } else if (type == sinkType::file) {
                sinks.push_back(quill::Frontend::create_or_get_sink<quill::FileSink>(
                                identifier
                                )
                        );
            }
        }

        const quill::PatternFormatterOptions format{
                config.messagePattern,
                config.timeFormat,
                quill::Timezone::GmtTime
        };

        logger_ = quill::Frontend::create_or_get_logger(config.name, std::move(sinks), format);
        logger_->set_log_level(toQuillLevel(config.defaultLevel));
    }

    void LoggerImpl::log(const logLevel level, const char *fmt, auto &&... args) {
        if (logger_) [[likely]] {
            QUILL_LOG_DYNAMIC(logger_, toQuillLevel(level), fmt,
                              std::forward<decltype(args)>(args)...
                    );
        }
    }

    void LoggerImpl::setLevel(const logLevel level) const {
        if (logger_) {
            logger_->set_log_level(toQuillLevel(level));
        }
    }

    void LoggerImpl::flush() const {
        if (logger_) [[likely]]{
            logger_->flush_log();
        }
    }
}
