#include "Simulation/Simulation.hpp"

#include <cmath>

#include "Node/Node.hpp"
#include "Wire/Wire.hpp"
#include "Input/Mouse.hpp"
#include "Globals.hpp"

static std::array<Wire, 3> wires{};
static bool firstNode = true;

void Simulation::AddNode(const Node& node) {
    [[maybe_unused]] NodeHandle nodeHandle = m_nodes.AddElement(node);
    if (!firstNode) {

        std::vector<NodeHandle> nodes;
        m_nodes.foreach([&nodes](NodeHandle handle, Node&) {
                nodes.push_back(handle);
            });
        wires[0].setInput(nodes[0]);
        wires[0].setOutput(nodes[1]);

        if (nodes.size() >= 3) {
            wires[1].setInput(nodes[2]);
            wires[1].setOutput(nodes[3]);

            wires[2].setInput(nodes[1]);
            wires[2].setOutput(nodes[2]);
        }
    }
    firstNode = false;
}

void Simulation::AddNode(Node&& node) {
    [[maybe_unused]] NodeHandle nodeHandle = m_nodes.AddElement(std::forward<Node>(node));
    if (!firstNode) {

        std::vector<NodeHandle> nodes;
        m_nodes.foreach([&nodes](NodeHandle handle, Node&) {
                nodes.push_back(handle);
            });
        wires[0].setInput(nodes[0]);
        wires[0].setOutput(nodes[1]);

        if (nodes.size() >= 3) {
            wires[1].setInput(nodes[2]);
            wires[1].setOutput(nodes[3]);

            wires[2].setInput(nodes[1]);
            wires[2].setOutput(nodes[2]);
        }
    }
    firstNode = false;
}

void Simulation::Update() {
    float currentTime = m_clock.getElapsedTime().asSeconds();

    if (g_mouseButtons.at(sf::Mouse::Button::Left).startedPress()) LeftMousePressed();
    if (g_mouseButtons.at(sf::Mouse::Button::Left).released()) LeftMouseReleased();
    if (g_mouseButtons.at(sf::Mouse::Button::Left).held()) LeftMouseHold();

    if (g_mouseButtons.at(sf::Mouse::Button::Right).startedPress()) RightMousePressed();
    if (g_mouseButtons.at(sf::Mouse::Button::Right).released()) RightMouseReleased();

    float dt = currentTime - m_lastTime;
    for (Wire& wire : wires) {
        wire.Update(m_nodes, dt);
    }

    m_lastTime = currentTime; 
}

void Simulation::Render(sf::RenderTarget& target) {
    for (Wire& wire : wires) {
        wire.Render(target, m_nodes);
    }

    m_nodes.foreach([&target](NodeHandle, const Node& node) {
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
        [&topHeld] (NodeHandle, Node& node) {
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
        [&topHeld] (NodeHandle, Node& node) {
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
