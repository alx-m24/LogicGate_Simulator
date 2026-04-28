#pragma once

#include <cstdint>
#include <SFML/System/Vector2.hpp>

#include "Input/Mouse.hpp"

// === VARIABLE ===
extern float g_ViewportScale;
extern MouseButtons_T g_mouseButtons;
extern sf::Vector2i g_mousePosition;
extern sf::Vector2f g_mouseWorldPosition;
extern sf::Vector2f g_worldOffset;
extern sf::Vector2f g_mouseDelta;

// === CONSTANTS ===
inline constexpr float MOUSE_SENS = 0.5f;
inline constexpr float BACKGROUND_BASE_SCALE = 1 / 2.0f;
inline constexpr float ZOOM_SENSITIVIY = 0.125f;
inline constexpr float BASE_RADIUS = 25.0f;
inline constexpr uint8_t NODE_RESOLUTION = 20u;
inline constexpr float NODE_OUTLINE_THICKNESS = 5.0f;
