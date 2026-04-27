#include "Simulation/Simulation.hpp"

#include "Node/Node.hpp"

void Simulation::AddNode(const Node& node) {
    [[maybe_unused]] NodeHandle nodeHandle = m_nodes.AddElement(node);
}

void Simulation::AddNode(Node&& node) {
    [[maybe_unused]] NodeHandle nodeHandle = m_nodes.AddElement(std::forward<Node>(node));
}

void Simulation::Update() {
   float currentTime = m_clock.getElapsedTime().asSeconds();
   if (static_cast<uint32_t>(currentTime) != static_cast<uint32_t>(m_lastTime)) {
           m_nodes.foreach([](Node& node) {
                   node.toggleState();
                });
        }

    m_lastTime = currentTime; 
}

void Simulation::Render(sf::RenderTarget& target) {
    m_nodes.foreach([&target](const Node& node) {
                node.Draw(target); 
            });
}
