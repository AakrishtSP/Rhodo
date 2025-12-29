module;
#include <functional>
#include <source_location>
export module Rhodo.Debug:Config;
namespace rhodo {
export inline constexpr bool kIsDebug =
#ifdef RHODO_DEBUG
    true;
#else
    false;
#endif
namespace debug {
using HandlerT = std::function<void(
    const char* expr, const std::source_location& loc, const char* message)>;
}  // namespace debug
}  // namespace rhodo
