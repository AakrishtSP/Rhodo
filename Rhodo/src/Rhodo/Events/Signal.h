// Rhodo/src/Rhodo/Events/Signal.h
#pragma once
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <utility>
#include <vector>

namespace Rhodo
{
    class ISignal
    {
        virtual ~ISignal() = default;
        std::type_index type;
        std::string name;
    protected:
        explicit ISignal(const std::type_index t, std::string  name) : type(t), name(std::move(name)) {}
    };

    template<typename... Args>
    class Signal final : public ISignal
    {
        static std::unordered_map<std::string,std::unique_ptr<Signal>> signals;

    public:
        static Signal& getOrCreate(const std::string& name) {
            auto it = signals.find(name);
            if (it != signals.end()) {
                return *it->second;
            }
            auto signal = std::make_unique<Signal>(name);
            auto& ref = *signal;
            signals.emplace(name, std::move(signal));
            return ref;
        }
        static Signal& get(const std::string& name) {
            return *signals.at(name);
        }
        static bool exists(const std::string& name) {
            return signals.contains(name);
        }

        static void remove(const std::string& name) {
            signals.erase(name);
        }
    private:
        using Slot = std::function<void(Args...)>;
        std::vector<Slot> slots;
    public:
        explicit Signal(const std::string& name) : ISignal(typeid(Signal<Args...>), name) {}
        ~Signal() override = default;


        void connect(Slot s) { slots.push_back(std::move(s)); }
        void emit(Args... args) { for (auto &s : slots) s(args...); }
    };

    class SignalManager
    {
    public:
        template<typename... Args>
        static Signal<Args...>& getSignal(const std::string& name) {
            return Signal<Args...>::get(name);
        }

        template<typename... Args>
        static Signal<Args...>& createSignal(const std::string& name) {
            return Signal<Args...>::create(name);
        }
    };
}
