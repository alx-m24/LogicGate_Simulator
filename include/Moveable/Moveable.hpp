#pragma once

#include <SFML/System/Vector2.hpp>

class Moveable {
    protected:
        sf::Vector2f m_position{};

    public:
        Moveable() = default;
        Moveable(sf::Vector2f position) : m_position(position) {}
        virtual ~Moveable() = default;

    public:
        virtual void setPosition(sf::Vector2f position) {
            m_position = position;
        }

        virtual sf::Vector2f getPosition() const {
            return m_position;
        }
};
