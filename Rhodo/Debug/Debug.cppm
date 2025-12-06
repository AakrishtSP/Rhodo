module;

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <source_location>
#include <type_traits>
#include <utility>

export module Rhodo.Asserts;

import Rhodo.Logger;


export namespace Rhodo::Asserts {

    // ============ Build Configuration ============
#if defined(RHODO_DEBUG)
    inline constexpr bool isDebug = true;
#else
    inline constexpr bool isDebug = false;
#endif

    // ============ Handler Types ============
    using handlerT = void (*)(const char* expr, const std::source_location& loc, const char* message) noexcept;

    // ============ Core Assertion Functions ============

    // Debug assertions - stripped in release builds
    void debugCheck(
            bool condition,
            const char* expr,
            const std::source_location& loc = std::source_location::current(),
            const char* message = nullptr) noexcept;

    // Verify - reports but doesn't abort (always enabled)
    void verifyCheck(
            bool condition,
            const char* expr,
            const std::source_location& loc = std::source_location::current(),
            const char* message = nullptr) noexcept;

    // Ensure - like to verify but for critical runtime checks
    void ensureCheck(
            bool condition,
            const char* expr,
            const std::source_location& loc = std::source_location::current(),
            const char* message = nullptr) noexcept;

    // Contract programming support
    void precondition(
            bool condition,
            const char* expr,
            const std::source_location& loc = std::source_location::current(),
            const char* message = nullptr) noexcept;

    void postcondition(
            bool condition,
            const char* expr,
            const std::source_location& loc = std::source_location::current(),
            const char* message = nullptr) noexcept;

    void invariant(
            bool condition,
            const char* expr,
            const std::source_location& loc = std::source_location::current(),
            const char* message = nullptr) noexcept;

    // Unconditional panic - always aborts
    [[noreturn]] void panic(
            const char* message,
            const std::source_location& loc = std::source_location::current()) noexcept;

    // ============ Handler Management ============
    void setPanicHandler(handlerT h) noexcept;
    void setReportHandler(handlerT h) noexcept;

    handlerT getPanicHandler() noexcept;
    handlerT getReportHandler() noexcept;

    // ============ Performance Profiling ============
    class ScopedTimer {
    public:
        explicit ScopedTimer(
                const char* name,
                const std::source_location& loc = std::source_location::current()) noexcept
                : name_(name), loc_(loc), start_(std::chrono::high_resolution_clock::now()) {
        }

        ~ScopedTimer() noexcept {
            if constexpr(isDebug) {
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_);

                Logger::coreLogger::debug(
                        "[TIMER] {}: {} μs (at {}:{})",
                        name_,
                        duration.count(),
                        loc_.file_name(),
                        loc_.line());
            }
        }

        ScopedTimer(const ScopedTimer&) = delete;
        ScopedTimer& operator=(const ScopedTimer&) = delete;

    private:
        const char* name_;
        std::source_location loc_;
        std::chrono::high_resolution_clock::time_point start_;
    };

    // ============ Scope Guards ============
    template<typename F>
    struct ScopeExit {
        F fn;
        bool active = true;

        explicit ScopeExit(F f) noexcept(std::is_nothrow_move_constructible_v<F>)
                : fn(std::move(f)) {
        }

        ~ScopeExit() noexcept {
            if(active) fn();
        }

        void dismiss() noexcept { active = false; }

        ScopeExit(ScopeExit&& other) noexcept(std::is_nothrow_move_constructible_v<F>)
                : fn(std::move(other.fn)), active(other.active) {
            other.active = false;
        }

        ScopeExit(const ScopeExit&) = delete;
        ScopeExit& operator=(const ScopeExit&) = delete;
        ScopeExit& operator=(ScopeExit&&) = delete;
    };

    template<typename F>
    ScopeExit(F) -> ScopeExit<F>;

    template<typename F>
    struct ScopeFail {
        F fn;
        int exceptionsOnEntry;

        explicit ScopeFail(F f) noexcept(std::is_nothrow_move_constructible_v<F>)
                : fn(std::move(f)), exceptionsOnEntry(std::uncaught_exceptions()) {
        }

        ~ScopeFail() noexcept {
            if(std::uncaught_exceptions() > exceptionsOnEntry) {
                fn();
            }
        }

        ScopeFail(const ScopeFail&) = delete;
        ScopeFail& operator=(const ScopeFail&) = delete;
    };

    template<typename F>
    ScopeFail(F) -> ScopeFail<F>;

    template<typename F>
    struct ScopeSuccess {
        F fn;
        int exceptionsOnEntry;

        explicit ScopeSuccess(F f) noexcept(std::is_nothrow_move_constructible_v<F>)
                : fn(std::move(f)), exceptionsOnEntry(std::uncaught_exceptions()) {
        }

        ~ScopeSuccess() noexcept(noexcept(fn())) {
            if(std::uncaught_exceptions() <= exceptionsOnEntry) {
                fn();
            }
        }

        ScopeSuccess(const ScopeSuccess&) = delete;
        ScopeSuccess& operator=(const ScopeSuccess&) = delete;
    };

    template<typename F>
    ScopeSuccess(F) -> ScopeSuccess<F>;

    // ============ Memory Debugging ============
    class MemoryTracker {
    public:
        static void recordAllocation(
                void* ptr,
                size_t size,
                const std::source_location& loc = std::source_location::current()) noexcept {
            if constexpr(isDebug) {
                totalAllocated_.fetch_add(size, std::memory_order_relaxed);
                allocationCount_.fetch_add(1, std::memory_order_relaxed);
            }
        }

        static void recordDeallocation(void* ptr, size_t size) noexcept {
            if constexpr(isDebug) {
                totalFreed_.fetch_add(size, std::memory_order_relaxed);
                deallocationCount_.fetch_add(1, std::memory_order_relaxed);
            }
        }

        static size_t totalAllocated() noexcept { return totalAllocated_.load(std::memory_order_relaxed); }

        static size_t totalFreed() noexcept { return totalFreed_.load(std::memory_order_relaxed); }

        static size_t netAllocated() noexcept { return totalAllocated() - totalFreed(); }

        static size_t allocationCount() noexcept { return allocationCount_.load(std::memory_order_relaxed); }

        static size_t deallocationCount() noexcept { return deallocationCount_.load(std::memory_order_relaxed); }

        static void reset() noexcept {
            totalAllocated_.store(0, std::memory_order_relaxed);
            totalFreed_.store(0, std::memory_order_relaxed);
            allocationCount_.store(0, std::memory_order_relaxed);
            deallocationCount_.store(0, std::memory_order_relaxed);
        }

        static void printStats() noexcept {
            Logger::coreLogger::info(
                    "[MEMORY] Allocated: {} bytes ({} allocs), "
                    "Freed: {} bytes ({} frees), Net: {} bytes",
                    totalAllocated(),
                    allocationCount(),
                    totalFreed(),
                    deallocationCount(),
                    netAllocated());
        }

    private:
        static inline std::atomic<size_t> totalAllocated_{0};
        static inline std::atomic<size_t> totalFreed_{0};
        static inline std::atomic<size_t> allocationCount_{0};
        static inline std::atomic<size_t> deallocationCount_{0};
    };

}

// ==================== IMPLEMENTATION ====================

namespace Rhodo::Asserts::Detail {
    inline handlerT gPanicHandler = nullptr;
    inline handlerT gReportHandler = nullptr;
}

namespace Rhodo::Asserts {

    // ===== Default Handlers =====

    static void defaultPanicHandler(const char* expr, const std::source_location& loc, const char* message) noexcept {
        Logger::coreLogger::critical(
                "\n╔═══════════════════════════════════════════════════════════════╗\n"
                "║                        RHODO PANIC                            ║\n"
                "╚═══════════════════════════════════════════════════════════════╝\n"
                "Expression: {}\n"
                "Location:   {}:{}\n"
                "Function:   {}\n"
                "{}{}",
                expr ? expr : "(null)",
                loc.file_name(),
                loc.line(),
                loc.function_name(),
                message ? "Message:    " : "",
                message ? message : "");

        Logger::coreLogger::flush();
        std::abort();
    }

    static void defaultReportHandler(const char* expr, const std::source_location& loc, const char* message) noexcept {
        Logger::coreLogger::error(
                "[VERIFY FAILED] {} at {}:{}{}{}",
                expr ? expr : "(null)",
                loc.file_name(),
                loc.line(),
                message ? " - " : "",
                message ? message : "");
    }

    static inline void ensureDefaultsInstalled() noexcept {
        if(!Detail::gPanicHandler) Detail::gPanicHandler = &defaultPanicHandler;
        if(!Detail::gReportHandler) Detail::gReportHandler = &defaultReportHandler;
    }

    // ===== Handler Management =====

    void setPanicHandler(handlerT h) noexcept {
        ensureDefaultsInstalled();
        Detail::gPanicHandler = h ? h : &defaultPanicHandler;
    }

    void setReportHandler(handlerT h) noexcept {
        ensureDefaultsInstalled();
        Detail::gReportHandler = h ? h : &defaultReportHandler;
    }

    handlerT getPanicHandler() noexcept {
        ensureDefaultsInstalled();
        return Detail::gPanicHandler;
    }

    handlerT getReportHandler() noexcept {
        ensureDefaultsInstalled();
        return Detail::gReportHandler;
    }

    // ===== Core Assertions =====

    void debugCheck(const bool condition, const char* expr, const std::source_location& loc, const char* message) noexcept {
        if constexpr(!isDebug) {
            (void) condition;
            (void) expr;
            (void) loc;
            (void) message;
        } else {
            if(!condition) {
                ensureDefaultsInstalled();
                if(Detail::gPanicHandler) Detail::gPanicHandler(expr, loc, message);
                std::abort();
            }
        }
    }

    void verifyCheck(const bool condition, const char* expr, const std::source_location& loc, const char* message) noexcept {
        if(!condition) {
            ensureDefaultsInstalled();
            if(Detail::gReportHandler) Detail::gReportHandler(expr, loc, message);
        }
    }

    void ensureCheck(const bool condition, const char* expr, const std::source_location& loc, const char* message) noexcept {
        if(!condition) {
            ensureDefaultsInstalled();
            if(Detail::gPanicHandler) Detail::gPanicHandler(expr, loc, message);
            std::abort();
        }
    }

    void precondition(const bool condition, const char* expr, const std::source_location& loc, const char* message) noexcept {
        debugCheck(condition, expr, loc, message);
    }

    void postcondition(
            const bool condition,
            const char* expr,
            const std::source_location& loc,
            const char* message) noexcept {
        debugCheck(condition, expr, loc, message);
    }

    void invariant(const bool condition, const char* expr, const std::source_location& loc, const char* message) noexcept {
        debugCheck(condition, expr, loc, message);
    }

    [[noreturn]] void panic(const char* message, const std::source_location& loc) noexcept {
        ensureDefaultsInstalled();
        if(Detail::gPanicHandler) Detail::gPanicHandler("panic()", loc, message);
        std::abort();
    }

} // namespace Rhodo::Asserts