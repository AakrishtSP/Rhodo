// // Rhodo/src/Rhodo/Core/LayerStack.h
// #pragma once
// #include "Layer.h"
// #include "Rhodo/Core/Core.h"
// #include <vector>
//
// namespace Rhodo
// {
//     class LayerStack
//     {
//     public:
//         LayerStack() : m_layerInsertIndex(0)
//         {
//         };
//         ~LayerStack() = default;
//
//         // Disable copying.
//         LayerStack(const LayerStack&) = delete;
//         LayerStack& operator=(const LayerStack&) = delete;
//
//         void pushLayer(scope<Layer> layer);
//
//         void pushOverlay(scope<Layer> overlay);
//
//         void popLayer(Layer* layer);
//
//         void popOverlay(scope<Layer> overlay);
//
//         void onUpdate(float deltaTime);
//
//         void onRender(float deltaTime);
//
//         void onEvent(const Event& event);
//
//         [[nodiscard]] std::vector<scope<Layer>> getLayers() { return m_layers; }
//
//     private:
//         std::vector<scope<Layer>> m_layers;
//         unsigned int m_layerInsertIndex;
//     };
// } // Rhodo
