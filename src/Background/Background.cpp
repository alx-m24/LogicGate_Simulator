#include "Background/Background.hpp"

#include "Globals.hpp"

bool initialized = false;
sf::Texture backgroundTexture{};
sf::Sprite background{};
sf::Texture backgroundGridTexture{};
sf::Sprite backgroundGrid{};

void DrawBackground(sf::RenderTarget& target) {
    if (!initialized) {
        initialized = true;

        backgroundTexture.loadFromFile("res\\background.png");
        background.setTexture(backgroundTexture, true);

        backgroundGridTexture.loadFromFile("res\\background-grid_3.png");
        backgroundGridTexture.setRepeated(true);

        backgroundGrid.setTexture(backgroundGridTexture, true);
    }

    target.clear(sf::Color(56, 56, 56));

    sf::Vector2u windowSize = target.getSize();
    
    sf::Vector2f backgroundScale = sf::Vector2f(static_cast<float>(windowSize.x) / backgroundTexture.getSize().x, static_cast<float>(windowSize.y) / backgroundTexture.getSize().y);

    background.setPosition(0.0f, 0.0f);
    background.setScale(backgroundScale);
    target.draw(background);

    backgroundGrid.setTextureRect(sf::IntRect(
        static_cast<int>(-g_worldOffset.x / (BACKGROUND_BASE_SCALE * g_ViewportScale)),
        static_cast<int>(-g_worldOffset.y / (BACKGROUND_BASE_SCALE * g_ViewportScale)),
        static_cast<int>(windowSize.x / (BACKGROUND_BASE_SCALE * g_ViewportScale)),
        static_cast<int>(windowSize.y / (BACKGROUND_BASE_SCALE * g_ViewportScale))
    ));
    
    backgroundGrid.setScale(
        BACKGROUND_BASE_SCALE * g_ViewportScale,
        BACKGROUND_BASE_SCALE * g_ViewportScale
    );
    
    backgroundGrid.setPosition(0.f, 0.f);
    
    target.draw(backgroundGrid);
}
