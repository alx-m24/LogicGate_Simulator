#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>

template<typename Rep = float>
Rep Length(sf::Vector2f vec) {
    return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

template<typename Rep = float>
Rep Distance(sf::Vector2f a, sf::Vector2f b) {
    return Length(a - b);
}
