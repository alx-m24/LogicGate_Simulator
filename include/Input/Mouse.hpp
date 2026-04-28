#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>

class MouseButton {
    private:
        bool m_pressed = false;
        bool m_lastPressed = false;

    public:
        void Update() {
            m_lastPressed = m_pressed;
        }

        void OnPress() {
            m_pressed = true;
        }

        void OnRelease() {
            m_pressed = false;
        }
        
        bool isPressed() const {
            return m_pressed;
        }
        
        bool startedPress() const {
            return !m_lastPressed && m_pressed;
        }
        
        bool released() const {
            return m_lastPressed && !m_pressed;
        }

        bool held() const {
            return !startedPress() && isPressed();
        }
};

using MouseButtons_T = std::unordered_map<sf::Mouse::Button, MouseButton>;
