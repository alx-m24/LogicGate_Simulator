#include "Background/Background.hpp"

#include "Globals.hpp"

bool initialized = false;
sf::Texture backgroundTexture{};
sf::Sprite background{};

void DrawBackground(sf::RenderTarget& target) {
    if (!initialized) {
        initialized = true;
        backgroundTexture.loadFromFile("res\\background.png");
        backgroundTexture.setRepeated(true);

        background.setTexture(backgroundTexture, true);
    }

    target.clear(sf::Color(56, 56, 56));

    sf::Vector2u windowSize = target.getSize();
    
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
    
    target.draw(background);
}
