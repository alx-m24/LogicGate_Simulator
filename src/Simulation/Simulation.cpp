#include "Simulation/Simulation.hpp"

#include "Node/Node.hpp"
#include "Input/Mouse.hpp"
#include "Globals.hpp"

void Simulation::AddNode(const Node& node) {
    [[maybe_unused]] NodeHandle nodeHandle = m_nodes.AddElement(node);
}

void Simulation::AddNode(Node&& node) {
    [[maybe_unused]] NodeHandle nodeHandle = m_nodes.AddElement(std::forward<Node>(node));
}

void Simulation::Update() {
    if (g_mouseButtons.at(sf::Mouse::Button::Left).startedPress()) LeftMousePressed();
    if (g_mouseButtons.at(sf::Mouse::Button::Left).released()) LeftMouseReleased();
    if (g_mouseButtons.at(sf::Mouse::Button::Left).held()) LeftMouseHold();

   float currentTime = m_clock.getElapsedTime().asSeconds();

    m_lastTime = currentTime; 
}

void Simulation::Render(sf::RenderTarget& target) {
    m_nodes.foreach([&target](const Node& node) {
                node.Draw(target); 
            });
}

void Simulation::LeftMouseHold() {

}
    
void Simulation::LeftMousePressed() {

}

void Simulation::LeftMouseReleased() {
    Node* topNode = nullptr;
    m_nodes.foreach(
        [&topNode] (Node& node) {
            if (node.contains(g_mousePosition)) {
                topNode = &node;
            }
        }
    );
    if (topNode) topNode->toggleState();
}
