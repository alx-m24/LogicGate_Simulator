#include "Node/Node.hpp"
#include "Globals.hpp"

#include <SFML/Graphics.hpp>

sf::CircleShape g_NodeSprite(BASE_RADIUS, NODE_RESOLUTION);

sf::Vector2f Node::getPosition() const {
    return m_position;
}

void Node::setPosition(sf::Vector2f position) {
    m_position = position;
}

bool Node::getState() const {
    return m_state;
}

void Node::setState(bool state) {
    m_state = state;
}

void Node::toggleState() {
    m_state = !m_state;
}
    
float Node::getRadius() {
    return BASE_RADIUS * g_ViewportScale;
}

bool Node::isVisible(const sf::Vector2f minBounds, const sf::Vector2f maxBounds) const {
    return m_position.x > minBounds.x - getRadius() && m_position.y > minBounds.y - getRadius() && m_position.x < maxBounds.x + getRadius() && m_position.y < maxBounds.y + getRadius();
}

// Draws and handles culling
void Node::Draw(sf::RenderTarget& target) const {
    if (!isVisible({ 0.0f, 0.0f }, sf::Vector2f(target.getSize()))) return; // culling

    g_NodeSprite.setOutlineThickness(NODE_OUTLINE_THICKNESS);
    g_NodeSprite.setOutlineColor(sf::Color::Black);
    g_NodeSprite.setRadius(getRadius());
    g_NodeSprite.setFillColor(this->getState() ? sf::Color::Red : sf::Color::White);
    g_NodeSprite.setPosition(m_position);

    target.draw(g_NodeSprite);
}
