#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <math.h>

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
    simulation.AddNode({ sf::Vector2f(window.getSize().x, window.getSize().y / 2.0f) / 2.0f, true });

    g_mousePosition = sf::Mouse::getPosition(window);

    while (window.isOpen()) {
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
                g_mousePosition = sf::Mouse::getPosition(window);
            }
        }

        simulation.Update();

        window.clear(sf::Color(56, 56, 56));

        simulation.Render(window);

        window.display();
    }

    return EXIT_SUCCESS;
}
