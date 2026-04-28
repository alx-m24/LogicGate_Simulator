#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <cmath>

#include "Globals.hpp"
#include "Input/Mouse.hpp"
#include "Simulation/Simulation.hpp"

int main() {
    sf::Clock clock;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Logic Gate Simulator", sf::Style::Default, settings);

    window.setVerticalSyncEnabled(true);

    Simulation simulation{};

    simulation.AddNode({ sf::Vector2f(window.getSize()) / 2.0f, false });
    simulation.AddNode({ sf::Vector2f(window.getSize().x, window.getSize().y / 2.0f) / 2.0f, false });

    g_mousePosition = sf::Mouse::getPosition(window);

    sf::Texture backgroundTexture{};
    backgroundTexture.loadFromFile("res\\background.png");
    backgroundTexture.setRepeated(true);

    sf::Sprite background{};
    background.setTexture(backgroundTexture, true);

    while (window.isOpen()) {
        g_mouseDelta = {};
        for (auto& [buttonType, button] : g_mouseButtons) {
            button.Update();
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea({0.f, 0.f}, sf::Vector2f(event.size.width, event.size.height));
                window.setView(sf::View(visibleArea));
            }
            if (event.type == sf::Event::MouseWheelScrolled) {
                // g_ViewportScale += event.mouseWheelScroll.delta * ZOOM_SENSITIVIY; // LINEAR Zoom
                float factor = 1.0f + event.mouseWheelScroll.delta * ZOOM_SENSITIVIY;
                g_ViewportScale *= factor;
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                g_mouseButtons[event.mouseButton.button].OnPress();
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                g_mouseButtons[event.mouseButton.button].OnRelease();
            }
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2i newMousePosition = sf::Mouse::getPosition(window);
                g_mouseDelta = sf::Vector2f(g_mousePosition - newMousePosition) * MOUSE_SENS;
                g_mousePosition = newMousePosition;
            }
        }
        g_mouseWorldPosition = sf::Vector2f(g_mousePosition) - g_worldOffset;

        simulation.Update();

        window.clear(sf::Color(56, 56, 56));

        sf::Vector2u windowSize = window.getSize();
        
        background.setTextureRect(sf::IntRect(
            static_cast<int>(-g_worldOffset.x / (BACKGROUND_BASE_SCALE * g_ViewportScale)),
            static_cast<int>(-g_worldOffset.y / (BACKGROUND_BASE_SCALE * g_ViewportScale)),
            static_cast<int>(windowSize.x / (BACKGROUND_BASE_SCALE * g_ViewportScale)),
            static_cast<int>(windowSize.y / (BACKGROUND_BASE_SCALE * g_ViewportScale))
        ));
        
        background.setScale(
            BACKGROUND_BASE_SCALE * g_ViewportScale,
            BACKGROUND_BASE_SCALE * g_ViewportScale
        );
        
        background.setPosition(0.f, 0.f);
        
        window.draw(background);

        simulation.Render(window);

        window.display();
    }

    return EXIT_SUCCESS;
}
