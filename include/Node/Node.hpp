#pragma once

#include <SFML/Graphics.hpp>

class Node {
    public:
        Node() = default;
        Node(sf::Vector2f position, bool state = false) : m_state(state), m_position(position) {}

    private:
        bool m_state = false;
        sf::Vector2f m_position{};

    public:
        sf::Vector2f getPosition() const;
        void setPosition(sf::Vector2f position);

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
