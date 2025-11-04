// // Rhodo/src/Rhodo/Core/LayerStack.cpp
// #include "LayerStack.h"
//
// namespace Rhodo
// {
//     void LayerStack::pushLayer(scope<Layer> layer)
//     {
//         if (!layer) return;
//         layer->onAttach();
//         m_layers.insert(m_layers.begin() + m_layerInsertIndex, std::move(layer));
//         m_layerInsertIndex++;
//     }
//
//     void LayerStack::pushOverlay(scope<Layer> overlay)
//     {
//         if (!overlay) return;
//         overlay->onAttach();
//         m_layers.push_back(std::move(overlay));
//     }
//
//     void LayerStack::popLayer(Layer* layer)
//     {
//         if (const auto it = std::find_if(m_layers.begin(), m_layers.begin() + m_layerInsertIndex,
//                                          [layer](const scope<Layer>& ptr)
//                                          {
//                                              return ptr.get() == layer;
//                                          }); it != m_layers.
//             begin() + m_layerInsertIndex)
//         {
//             (*it)->onDetach();
//             m_layers.erase(it);
//             m_layerInsertIndex--;
//         }
//     }
//
//     void LayerStack::popOverlay(scope<Layer> overlay)
//     {
//     }
//
//     void LayerStack::onUpdate(float deltaTime)
//     {
//     }
//
//     void LayerStack::onRender(float deltaTime)
//     {
//     }
//
//     void LayerStack::onEvent(const Event& event)
//     {
//     }
// } // Rhodo
