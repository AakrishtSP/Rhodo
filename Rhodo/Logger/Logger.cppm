export module Rhodo.Logger;

export namespace Rhodo::Logger
{
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

    // Initialization
    auto init() -> void;

    // Runtime log level switching
    auto set_core_level(log_level level) -> void;
    auto set_app_level (log_level level)  -> void;

    // Normal logging (fmt-style)
    auto trace_l3(const char *fmt, auto &&... args) -> void;
    auto trace_l2(const char *fmt, auto &&... args) -> void;
    auto trace_l1(const char *fmt, auto &&... args) -> void;
    auto debug(const char *fmt, auto &&... args) -> void;
    auto info(const char *fmt, auto &&... args) -> void;
    auto notice(const char *fmt, auto &&... args) -> void;
    auto warn(const char *fmt, auto &&... args) -> void;
    auto error(const char *fmt, auto &&... args) -> void;
    auto critical(const char *fmt, auto &&... args) -> void;
    auto backtrace(const char *fmt, auto &&... args) -> void;

    // Core logger versions
    auto trace_l3_core(const char *fmt, auto &&... args) -> void;
    auto trace_l2_core(const char *fmt, auto &&... args) -> void;
    auto trace_l1_core(const char *fmt, auto &&... args) -> void;
    auto debug_core(const char *fmt, auto &&... args) -> void;
    auto info_core(const char *fmt, auto &&... args) -> void;
    auto notice_core(const char *fmt, auto &&... args) -> void;
    auto warn_core(const char *fmt, auto &&... args) -> void;
    auto error_core(const char *fmt, auto &&... args) -> void;
    auto critical_core(const char *fmt, auto &&... args) -> void;
    auto backtrace_core(const char *fmt, auto &&... args) -> void;

    // Value logging (LOGV equivalent)
    auto vdebug(const char *msg, auto &&... vars) -> void;
    auto vinfo(const char *msg, auto &&... vars) -> void;
    auto vwarn(const char *msg, auto &&... vars) -> void;
    auto verror(const char *msg, auto &&... vars) -> void;

    auto vdebug_core(const char *msg, auto &&... vars) -> void;
    auto vinfo_core(const char *msg, auto &&... vars) -> void;
    auto vwarn_core(const char *msg, auto &&... vars) -> void;
    auto verror_core(const char *msg, auto &&... vars) -> void;
}

