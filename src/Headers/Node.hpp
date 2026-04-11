#pragma once
#include <SFML/Graphics.hpp>


class Node
{
public:
	bool state = false;
	bool selected = false;
	bool shouldDraw = true;
	sf::Vector2f position;
	sf::Color activeColor = sf::Color(219, 22, 47);

private:
	sf::CircleShape circle;

public:
	void draw(sf::RenderWindow& window, float radius);

public:
	inline bool contains(sf::Vector2f point) { return circle.getGlobalBounds().contains(point); }
};
