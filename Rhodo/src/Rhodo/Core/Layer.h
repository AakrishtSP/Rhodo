// // Rhodo/src/Rhodo/Core/Layer.h
// #pragma once
// #include <utility>
//
// #include "Rhodo/Events/Event.h"
//
// namespace Rhodo {
//
// class Layer {
// public:
//     explicit Layer(std::string layerName="Layer") : m_debugName(std::move(layerName)) {};
//
//     virtual ~Layer() = default;
//
//     virtual void onAttach() = 0;
//
//     virtual void onDetach() = 0;
//
//     virtual void onUpdate() = 0;
//
//     virtual void onEvent(Event& event) = 0;
//
//     [[nodiscard]] const std::string& getName() const {return m_debugName;}
//
// private:
//     std::string m_debugName;
// };
//
// } // Rhodo
