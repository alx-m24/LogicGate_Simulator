#include "Globals.hpp"

float g_ViewportScale = 1.0f;
MouseButtons_T g_mouseButtons{
    { sf::Mouse::Left, {} },
    { sf::Mouse::Middle, {} },
    { sf::Mouse::Right, {} },
};
sf::Vector2i g_mousePosition{};
sf::Vector2f g_mouseWorldPosition{};
sf::Vector2f g_worldOffset{};
sf::Vector2f g_mouseDelta{};
