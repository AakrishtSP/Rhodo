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
    auto set_app_level(log_level level)  -> void;

    // Normal logging (fmt-style)
    void trace_l3(const char* fmt, auto&&... args);
    void trace_l2(const char* fmt, auto&&... args);
    void trace_l1(const char* fmt, auto&&... args);
    void debug   (const char* fmt, auto&&... args);
    void info    (const char* fmt, auto&&... args);
    void notice  (const char* fmt, auto&&... args);
    void warn    (const char* fmt, auto&&... args);
    void error   (const char* fmt, auto&&... args);
    void critical(const char* fmt, auto&&... args);
    void backtrace(const char* fmt, auto&&... args);

    // Core logger versions
    void trace_l3_core(const char* fmt, auto&&... args);
    void trace_l2_core(const char* fmt, auto&&... args);
    void trace_l1_core(const char* fmt, auto&&... args);
    void debug_core   (const char* fmt, auto&&... args);
    void info_core    (const char* fmt, auto&&... args);
    void notice_core  (const char* fmt, auto&&... args);
    void warn_core    (const char* fmt, auto&&... args);
    void error_core   (const char* fmt, auto&&... args);
    void critical_core(const char* fmt, auto&&... args);
    void backtrace_core(const char* fmt, auto&&... args);

    // Value logging (LOGV equivalent)
    void vdebug(const char* msg, auto&&... vars);
    void vinfo (const char* msg, auto&&... vars);
    void vwarn (const char* msg, auto&&... vars);
    void verror(const char* msg, auto&&... vars);

    void vdebug_core(const char* msg, auto&&... vars);
    void vinfo_core (const char* msg, auto&&... vars);
    void vwarn_core (const char* msg, auto&&... vars);
    void verror_core(const char* msg, auto&&... vars);
}

