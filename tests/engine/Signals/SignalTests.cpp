// #define CATCH_CONFIG_MAIN
#include <atomic>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include "catch2/catch_test_macros.hpp"
import Rhodo.Signals;

using namespace rhodo;
using namespace std::chrono_literals;

// -----------------------------------------------------------------------------
// Helper utilities
// -----------------------------------------------------------------------------
template <typename T>
struct Counter {
  std::atomic<T> value{0};
  void operator()(T v) { value.fetch_add(v, std::memory_order_relaxed); }
  [[nodiscard]] auto Get() const -> T {
    return value.load(std::memory_order_relaxed);
  }
};

// -----------------------------------------------------------------------------
// Basic Signal functionality
// -----------------------------------------------------------------------------
TEST_CASE("Signal basic connect/emit/disconnect", "[Signal]") {
  Signal<int> sig;
  int called = 0;

  auto id = sig.connect([&called](int v) -> void { called += v; });
  sig.emit(5);
  REQUIRE(called == 5);

  sig.disconnect(id);
  sig.emit(10);
  REQUIRE(called == 5);  // no further calls
}

// -----------------------------------------------------------------------------
// ScopedConnection RAII
// -----------------------------------------------------------------------------
TEST_CASE("ScopedConnection auto-disconnects", "[ScopedConnection]") {
  Signal<> sig;
  int const kCount = 0;

  {
    [[maybe_unused]] auto conn = Signals::makeScopedConnection(
        sig, std::function<void()>{[&count] { ++count; }});
    sig.emit();
    REQUIRE(kCount == 1);
  }  // conn destroyed here

  sig.emit();
  REQUIRE(kCount == 1);  // still 1
}

// -----------------------------------------------------------------------------
// Member-function connection
// -----------------------------------------------------------------------------
struct Receiver {
  int sum = 0;
  void Add(int v) { sum += v; }
};

TEST_CASE("Signal member function connection", "[Signal]") {
  Signal<int> sig;
  Receiver r;

  auto id = sig.connect(r, &Receiver::Add);
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

  sig.connect([&result](const std::string& s) -> void { result = s; });
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

  auto id = sig.connect([] -> void {});
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

  sig.connect([&logs](int v) -> void { logs.push_back(v); });
  sig.connect([&logs](int v) -> void { logs.push_back(v * 2); });

  sig.emit(1);
  REQUIRE(logs == std::vector<int>{1, 2});

  sig.disconnectAll();
  sig.emit(10);
  REQUIRE(logs == std::vector<int>{1, 2});  // no new entries
}

// -----------------------------------------------------------------------------
// blocking_emit()
// -----------------------------------------------------------------------------
TEST_CASE("Signal blocking_emit guarantees delivery", "[Signal]") {
  Signal<> sig;
  std::atomic<int> counter{0};

  sig.connect([&] -> void { ++counter; });
  sig.blockingEmit();
  REQUIRE(counter == 1);

  // Even after disconnect_all, blocking_emit still sees current slots
  sig.disconnectAll();
  sig.blockingEmit();  // no effect
  REQUIRE(counter == 1);
}

// -----------------------------------------------------------------------------
// Concurrent emission (shared lock)
// -----------------------------------------------------------------------------
TEST_CASE("Signal concurrent emit is safe", "[Signal][thread]") {
  Signal<int> sig;
  Counter<int> total;

  sig.connect([&total](int x) -> void { total(x); });

  constexpr int kN = 100'000;
  std::vector<std::thread> threads;
  for (int i = 0; i < 4; ++i) {
    threads.emplace_back([&sig, kN] -> void {
      for (int j = 0; j < kN; ++j) {
        sig.emit(1);
      }
    });
  }

  for (auto& t : threads)
    t.join();
  REQUIRE(total.get() == 4 * kN);
}

// -----------------------------------------------------------------------------
// Concurrent connect/disconnect while emitting
// -----------------------------------------------------------------------------
TEST_CASE("Signal connect/disconnect while emitting", "[Signal][thread]") {
  Signal<> sig;
  std::atomic<int> emitted{0};
  std::atomic<bool> stop{false};

  auto id = sig.connect([&] -> void { ++emitted; });

  std::thread emitter([&] -> void {
    while (!stop) {
      sig.emit();
    }
  });

  // Hammer connect/disconnect
  for (int i = 0; i < 50'000; ++i) {
    auto new_id = sig.connect([] -> void {});
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
  constexpr uint32_t kThresh = Signal<>::cleanup_threshold = 0;

  std::vector<slotId> ids;
  for (uint32_t i = 0; i < kThresh * 2; ++i) {
    ids.push_back(sig.connect([] -> void {}));
  }

  // Disconnect half + 1 → should trigger cleanup on the next emit
  for (uint32_t i = 0; i < kThresh + 1; ++i) {
    sig.disconnect(ids[i]);
  }

  // Force an emit → guard will clean up
  sig.emit();

  // After cleanup, slots_ should contain only active ones
  REQUIRE(sig.size() == ids.size() - (kThresh + 1));
}

// -----------------------------------------------------------------------------
// force_cleanup()
// -----------------------------------------------------------------------------
TEST_CASE("Signal force_cleanup removes dead slots immediately", "[Signal]") {
  Signal<> sig;
  auto id = sig.connect([] -> void {});
  sig.disconnect(id);
  REQUIRE(sig.containerSize() == 1);  // still in container

  sig.forceCleanup();
  REQUIRE(sig.containerSize() == 0);
}

// -----------------------------------------------------------------------------
// SignalHub & global Signals
// -----------------------------------------------------------------------------
TEST_CASE("SignalHub get/create/remove", "[SignalHub]") {
  auto& hub = Signals::global();

  [[maybe_unused]] auto& s1 = hub.get<int>("test");
  REQUIRE(Signals::has<int>("test"));
  REQUIRE(hub.size() == 1);

  hub.remove<int>("test");
  REQUIRE(!Signals::has<int>("test"));
  REQUIRE(hub.size() == 0);
}

TEST_CASE("SignalHub type safety", "[SignalHub]") {
  auto& s_int = Signals::get<int>("mixed");
  auto& s_float = Signals::get<float>("mixed");

  REQUIRE(
      reinterpret_cast<void*>(&sInt) !=
      reinterpret_cast<void*>(&sFloat));  // different types → different signals
  REQUIRE(Signals::has<int>("mixed"));
  REQUIRE(Signals::has<float>("mixed"));
}

TEST_CASE("SignalHub cleanup_empty_signals", "[SignalHub]") {
  Signals::get<>("empty1");
  Signals::get<int>("empty2");
  REQUIRE(Signals::global().size() == 2);

  Signals::cleanupEmpty();  // removes empty signals
  REQUIRE(Signals::global().size() == 0);
}

// -----------------------------------------------------------------------------
// Global convenience functions
// -----------------------------------------------------------------------------
TEST_CASE("Global Signals helpers", "[Signals]") {
  auto& sig = Signals::get<std::string>("log");
  std::ostringstream oss;
  sig.connect([&oss](const std::string& m) -> void { oss << m; });

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
  sig.connect([](int) -> void { throw std::runtime_error("boom"); });
  sig.connect([](int v) -> void { /* ignore */ });

  REQUIRE_NOTHROW(sig.emit(0));  // one throws, other still called
  REQUIRE_NOTHROW(sig.blockingEmit(0));
}
