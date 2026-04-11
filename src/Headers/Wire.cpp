#include "Wire.hpp"

void Wire::update()
{
	if (input != nullptr && output != nullptr)
		*output = *input;
}

bool Wire::getState() const
{
	if (input != nullptr && output != nullptr)
		return *input;

	return false;
}

bool Wire::contains(sf::Vector2f point) const
{
    const float padding = 5.0f; // half-width of the wire

    sf::Vector2f line = *p2 - *p1;
    sf::Vector2f toPoint = point - *p1;

    float lineLengthSq = line.x * line.x + line.y * line.y;

    if (lineLengthSq == 0) return false; // degenerate wire

    // Project toPoint onto the wire direction
    float t = (toPoint.x * line.x + toPoint.y * line.y) / lineLengthSq;

    // Clamp t to the segment [0,1]
    if (t < 0 || t > 1) return false;

    // Closest point on the segment
    sf::Vector2f closest = *p1 + t * line;

    // Distance from point to wire
    sf::Vector2f diff = point - closest;
    float distSq = diff.x * diff.x + diff.y * diff.y;

    return distSq <= padding * padding;
}
