module;
#include <string>
#include <vector>

export module Rhodo.Logger:Structures;

namespace Rhodo::Logger {
    enum class logLevel {
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

    enum class sinkType {
        console,
        file
    };

    // Configuration structures
    struct SinkConfig {
        sinkType type;
        std::string identifier;
    };

    struct LoggerConfig {
        std::string name;
        std::vector<SinkConfig> sinks;
        std::string timeFormat = "%H:%M:%S.%Qms";
        std::string messagePattern = "%(time) [%(thread_id)] %(log_level:<9) %(logger:<12) %(message)";
        logLevel defaultLevel = logLevel::debug;
    };
}
