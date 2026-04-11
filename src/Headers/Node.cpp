#include "Node.hpp"

void Node::draw(sf::RenderWindow& window, float radius)
{
	if (!shouldDraw) return;

	circle.setRadius(radius);

	circle.setFillColor(state ? sf::Color(209, 0, 0) : sf::Color::White);
	if (selected) {
		sf::Color color = circle.getFillColor();
		circle.setFillColor(sf::Color(static_cast<sf::Uint8>(color.r / 2.0f),
			static_cast<sf::Uint8>(color.g / 2.0f),
			static_cast<sf::Uint8>(color.b / 2.0f)));
	}

	circle.setOutlineThickness(3.0f);
	circle.setOutlineColor(sf::Color::Black);
	circle.setOrigin(circle.getRadius(), circle.getRadius());
	circle.setPosition(position);

	window.draw(circle);

	selected = false;
}