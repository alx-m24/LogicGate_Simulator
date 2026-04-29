#pragma once

#include <SFML/Graphics.hpp>

#include "Moveable/Moveable.hpp"
#include "Holdable/Holdable.hpp"

class Node : public Moveable, public Holdable {
    public:
        Node() = default;
        Node(sf::Vector2f position, bool state = false) : Moveable(position), m_state(state) {}

    private:
        bool m_state = false;

    public:
        bool getState() const;
        void setState(bool state);

        void toggleState();

    public:
        bool operator()() const {
            return getState();
        }

        static float getRadius();

    public:
        bool isVisible(const sf::Vector2f minBounds, const sf::Vector2f maxBounds) const;

        bool contains(sf::Vector2f point) const;
        bool contains(sf::Vector2i point) const;

        // Draws and handles culling
        void Draw(sf::RenderTarget& target) const;
};
