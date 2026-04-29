#include "Simulation/Simulation.hpp"

#include <cmath>

#include "Node/Node.hpp"
#include "Wire/BezierCurve.hpp"
#include "Input/Mouse.hpp"
#include "Globals.hpp"

void Simulation::AddNode(const Node& node) {
    [[maybe_unused]] NodeHandle nodeHandle = m_nodes.AddElement(node);
}

void Simulation::AddNode(Node&& node) {
    [[maybe_unused]] NodeHandle nodeHandle = m_nodes.AddElement(std::forward<Node>(node));
}

void Simulation::Update() {
    float currentTime = m_clock.getElapsedTime().asSeconds();

    if (g_mouseButtons.at(sf::Mouse::Button::Left).startedPress()) LeftMousePressed();
    if (g_mouseButtons.at(sf::Mouse::Button::Left).released()) LeftMouseReleased();
    if (g_mouseButtons.at(sf::Mouse::Button::Left).held()) LeftMouseHold();

    if (g_mouseButtons.at(sf::Mouse::Button::Right).startedPress()) RightMousePressed();
    if (g_mouseButtons.at(sf::Mouse::Button::Right).released()) RightMouseReleased();

    m_lastTime = currentTime; 
}

void Simulation::Render(sf::RenderTarget& target) {
    std::vector<sf::Vector2f> nodePositions;
    m_nodes.foreach([&nodePositions](Node& node) {
                if (nodePositions.size() % 2 == 0) node.setState(true);
                nodePositions.push_back(node.getPosition());
            });

    for (size_t i = 0; i < nodePositions.size(); i += 2) {
        BezierCurve curve(30, 5.0f * g_ViewportScale);
        curve.SetPoints(
                // P1
                { nodePositions[i].x, nodePositions[i].y },
                // P2
                { nodePositions[i + 1].x, nodePositions[i + 1].y },
                // C1
                { nodePositions[i + 1].x, nodePositions[i].y },
                // C2
                { nodePositions[i].x, nodePositions[i + 1].y }
                );
        curve.SetColor(sf::Color::White, sf::Color::Red, std::fmod(m_clock.getElapsedTime().asSeconds(), 1.5f) / 1.0f);
        
        curve.Draw(target);
    }

    m_nodes.foreach([&target](const Node& node) {
                node.Draw(target); 
            });
}

void Simulation::LeftMouseHold() {
    if (!m_beingHeld) {
        g_worldOffset -= g_mouseDelta;
        return;
    }

    if (Moveable* moveable = dynamic_cast<Moveable*>(m_beingHeld)) {
        moveable->setPosition(g_mouseWorldPosition);
    }
}

void Simulation::LeftMousePressed() {
    Holdable* topHeld = nullptr;
    m_nodes.foreach(
        [&topHeld] (Node& node) {
            if (node.contains(g_mouseWorldPosition)) {
                topHeld = &node;
            }
        }
    );

    if (!topHeld) return;
    
    m_beingHeld = topHeld;
    m_beingHeld->held = true;
}

void Simulation::LeftMouseReleased() {
    if (m_beingHeld) {
        m_beingHeld->held = false;
        m_beingHeld = nullptr;
    }
}

void Simulation::RightMousePressed() {
    Holdable* topHeld = nullptr;
    m_nodes.foreach(
        [&topHeld] (Node& node) {
            if (node.contains(g_mouseWorldPosition)) {
                topHeld = &node;
            }
        }
    );

    if (!topHeld) return;
    
    m_beingHeld = topHeld;
    m_beingHeld->held = true;
}

void Simulation::RightMouseReleased() {
    if (m_beingHeld) {
        if (Node* node = dynamic_cast<Node*>(m_beingHeld)) {
            node->toggleState();
        }
        m_beingHeld->held = false;
        m_beingHeld = nullptr;
    }
}
