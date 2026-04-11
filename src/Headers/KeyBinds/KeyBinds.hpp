#pragma once
#include <SFML/Graphics.hpp>
#include <map>

extern std::map<sf::Keyboard::Key, bool> keys;

#define SPACE (keys[sf::Keyboard::Space])
#define CTRL (keys[sf::Keyboard::LControl] || keys[sf::Keyboard::RControl])
#define SHIFT (keys[sf::Keyboard::LShift] || keys[sf::Keyboard::RShift])
#define DELETE (keys[sf::Keyboard::Delete] || keys[sf::Keyboard::BackSpace])
#define LEFTMOUSE (sf::Mouse::isButtonPressed(sf::Mouse::Left))
#define RIGHTMOUSE (sf::Mouse::isButtonPressed(sf::Mouse::Right))
#define MIDDLEMOUSE (sf::Mouse::isButtonPressed(sf::Mouse::Middle))

#define ADD_NODE (keys[sf::Keyboard::N] && CTRL)
#define ADD_GATE (keys[sf::Keyboard::N] && CTRL && SHIFT)