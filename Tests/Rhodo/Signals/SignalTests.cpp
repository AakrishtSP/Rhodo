// #define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include <atomic>
#include <thread>
#include <chrono>
import Rhodo.Signals;

using namespace Rhodo;
using namespace std::chrono_literals;


// -----------------------------------------------------------------------------
// Helper utilities
// -----------------------------------------------------------------------------
template<typename T>
struct Counter {
    std::atomic<T> value{0};
    void operator()(T v) { value.fetch_add(v, std::memory_order_relaxed); }
    [[nodiscard]] T get() const { return value.load(std::memory_order_relaxed); }
};

// -----------------------------------------------------------------------------
// Basic Signal functionality
// -----------------------------------------------------------------------------
TEST_CASE("Signal basic connect/emit/disconnect", "[Signal]") {
    Signal<int> sig;
    int called = 0;

    auto id = sig.connect([&called](int v) { called += v; });
    sig.emit(5);
    REQUIRE(called == 5);

    sig.disconnect(id);
    sig.emit(10);
    REQUIRE(called == 5);               // no further calls
}

// -----------------------------------------------------------------------------
// ScopedConnection RAII
// -----------------------------------------------------------------------------
TEST_CASE("ScopedConnection auto-disconnects", "[ScopedConnection]") {
    Signal<> sig;
    int count = 0;

    {
        [[maybe_unused]] auto conn = Signals::make_scoped_connection(sig, std::function<void()>{ [&count] { ++count; } });
        sig.emit();
        REQUIRE(count == 1);
    }                                   // conn destroyed here

    sig.emit();
    REQUIRE(count == 1);                // still 1
}

// -----------------------------------------------------------------------------
// Member-function connection
// -----------------------------------------------------------------------------
struct Receiver {
    int sum = 0;
    void add(int v) { sum += v; }
};

TEST_CASE("Signal member function connection", "[Signal]") {
    Signal<int> sig;
    Receiver r;

    auto id = sig.connect(r, &Receiver::add);
    sig.emit(7);
    REQUIRE(r.sum == 7);

    sig.disconnect(id);
    sig.emit(3);
    REQUIRE(r.sum == 7);
}

// -----------------------------------------------------------------------------
// operator() alias
// -----------------------------------------------------------------------------
TEST_CASE("Signal operator() alias", "[Signal]") {
    Signal<std::string> sig;
    std::string result;

    sig.connect([&result](const std::string& s) { result = s; });
    sig("hello");
    REQUIRE(result == "hello");
}

// -----------------------------------------------------------------------------
// size() / empty()
// -----------------------------------------------------------------------------
TEST_CASE("Signal size and empty", "[Signal]") {
    Signal<> sig;
    REQUIRE(sig.empty());
    REQUIRE(sig.size() == 0);

    auto id = sig.connect([]{});
    REQUIRE(!sig.empty());
    REQUIRE(sig.size() == 1);

    sig.disconnect(id);
    REQUIRE(sig.empty());
    REQUIRE(sig.size() == 0);
}

// -----------------------------------------------------------------------------
// disconnect_all()
// -----------------------------------------------------------------------------
TEST_CASE("Signal disconnect_all", "[Signal]") {
    Signal<int> sig;
    std::vector<int> logs;

    sig.connect([&logs](int v){ logs.push_back(v); });
    sig.connect([&logs](int v){ logs.push_back(v*2); });

    sig.emit(1);
    REQUIRE(logs == std::vector<int>{1, 2});

    sig.disconnect_all();
    sig.emit(10);
    REQUIRE(logs == std::vector<int>{1, 2});   // no new entries
}

// -----------------------------------------------------------------------------
// blocking_emit()
// -----------------------------------------------------------------------------
TEST_CASE("Signal blocking_emit guarantees delivery", "[Signal]") {
    Signal<> sig;
    std::atomic<int> counter{0};

    sig.connect([&]{ ++counter; });
    sig.blocking_emit();
    REQUIRE(counter == 1);

    // Even after disconnect_all, blocking_emit still sees current slots
    sig.disconnect_all();
    sig.blocking_emit();               // no effect
    REQUIRE(counter == 1);
}

// -----------------------------------------------------------------------------
// Concurrent emission (shared lock)
// -----------------------------------------------------------------------------
TEST_CASE("Signal concurrent emit is safe", "[Signal][thread]") {
    Signal<int> sig;
    Counter<int> total;

    sig.connect([&total](int x) { total(x); });

    const int N = 100'000;
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&sig, N]{
            for (int j = 0; j < N; ++j) sig.emit(1);
        });
    }

    for (auto& t : threads) t.join();
    REQUIRE(total.get() == 4 * N);
}

// -----------------------------------------------------------------------------
// Concurrent connect/disconnect while emitting
// -----------------------------------------------------------------------------
TEST_CASE("Signal connect/disconnect while emitting", "[Signal][thread]") {
    Signal<> sig;
    std::atomic<int> emitted{0};
    std::atomic<bool> stop{false};

    auto id = sig.connect([&]{ ++emitted; });

    std::thread emitter([&]{
        while (!stop) sig.emit();
    });

    // Hammer connect/disconnect
    for (int i = 0; i < 50'000; ++i) {
        auto newId = sig.connect([]{});
        sig.disconnect(newId);
    }

    stop = true;
    emitter.join();
    sig.disconnect(id);
    REQUIRE(emitted > 0);
}

// -----------------------------------------------------------------------------
// Batched cleanup threshold
// -----------------------------------------------------------------------------
TEST_CASE("Signal batched cleanup respects threshold", "[Signal]") {
    Signal<> sig;
    constexpr uint32_t THRESH = Signal<>::cleanup_threshold;

    std::vector<Signal<>::slot_id> ids;
    for (uint32_t i = 0; i < THRESH * 2; ++i) {
        ids.push_back(sig.connect([]{}));
    }

    // Disconnect half + 1 → should trigger cleanup on next emit
    for (uint32_t i = 0; i < THRESH + 1; ++i) {
        sig.disconnect(ids[i]);
    }

    // Force an emit → guard will cleanup
    sig.emit();

    // After cleanup, slots_ should contain only active ones
    REQUIRE(sig.size() == ids.size() - (THRESH + 1));
}

// -----------------------------------------------------------------------------
// force_cleanup()
// -----------------------------------------------------------------------------
TEST_CASE("Signal force_cleanup removes dead slots immediately", "[Signal]") {
    Signal<> sig;
    auto id = sig.connect([]{});
    sig.disconnect(id);
    REQUIRE(sig.container_size() == 1);          // still in container

    sig.force_cleanup();
    REQUIRE(sig.container_size() == 0);
}

// -----------------------------------------------------------------------------
// SignalHub & global Signals
// -----------------------------------------------------------------------------
TEST_CASE("SignalHub get/create/remove", "[SignalHub]") {
    auto& hub = Signals::global();

    auto& s1 = hub.get<int>("test");
    REQUIRE(Signals::has<int>("test"));
    REQUIRE(hub.size() == 1);

    hub.remove<int>("test");
    REQUIRE(!Signals::has<int>("test"));
    REQUIRE(hub.size() == 0);
}

TEST_CASE("SignalHub type safety", "[SignalHub]") {
    auto& sInt   = Signals::get<int>("mixed");
    auto& sFloat = Signals::get<float>("mixed");

    REQUIRE(reinterpret_cast<void*>(&sInt) != reinterpret_cast<void*>(&sFloat));         // different types → different signals
    REQUIRE(Signals::has<int>("mixed"));
    REQUIRE(Signals::has<float>("mixed"));
}

TEST_CASE("SignalHub cleanup_empty_signals", "[SignalHub]") {
    Signals::get<>("empty1");
    Signals::get<int>("empty2");
    REQUIRE(Signals::global().size() == 2);

    Signals::cleanup_empty();          // removes empty signals
    REQUIRE(Signals::global().size() == 0);
}

// -----------------------------------------------------------------------------
// Global convenience functions
// -----------------------------------------------------------------------------
TEST_CASE("Global Signals helpers", "[Signals]") {
    auto& sig = Signals::get<std::string>("log");
    std::ostringstream oss;
    sig.connect([&oss](const std::string& m){ oss << m; });

    sig.emit("hello");
    REQUIRE(oss.str() == "hello");

    Signals::remove<std::string>("log");
    REQUIRE(!Signals::has<std::string>("log"));
}

// -----------------------------------------------------------------------------
// Exception safety – callbacks may throw
// -----------------------------------------------------------------------------
TEST_CASE("Signal exception safety", "[Signal]") {
    Signal<int> sig;
    sig.connect([](int){ throw std::runtime_error("boom"); });
    sig.connect([](int v){ /* ignore */ });

    REQUIRE_NOTHROW(sig.emit(0));      // one throws, other still called
    REQUIRE_NOTHROW(sig.blocking_emit(0));
}