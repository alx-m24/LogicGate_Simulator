#include <SFML/Graphics.hpp>
#include <math.h>

#include "Node/Node.hpp"
#include "Globals.hpp"

int main() {
    sf::Clock clock;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Logic Gate Simulator", sf::Style::Default, settings);

    window.setVerticalSyncEnabled(true);

    Node node{};
    node.setPosition(sf::Vector2f(window.getSize()) / 2.0f);
    node.toggleState();

    uint32_t lastTime = 0;

    while (window.isOpen()) {
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
        }

        window.clear(sf::Color(56, 56, 56));

        uint32_t currentTime = static_cast<uint32_t>(clock.getElapsedTime().asSeconds());
        if (currentTime != lastTime) node.toggleState();
        lastTime = currentTime; 

        node.Draw(window);

        window.display();
    }

    return EXIT_SUCCESS;
}
