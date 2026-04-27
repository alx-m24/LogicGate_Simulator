#pragma once

#include <SFML/Graphics.hpp>

#include "Node/Node.hpp"
#include "Storage/Storage.hpp"

class Simulation {
    private:
        using NodeStorage = Storage<Node>;
        using NodeHandle = NodeStorage::Handle_T;
        NodeStorage m_nodes{};

    public:
        sf::Clock m_clock{};
        float m_lastTime{};

    public:
        Simulation() = default;

    public:
        void Update();

        void Render(sf::RenderTarget& target);

    public:
        void AddNode(Node&& node);
        void AddNode(const Node& node);
};
