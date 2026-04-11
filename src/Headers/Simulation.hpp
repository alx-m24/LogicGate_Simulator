#pragma once
#include "CircuitFileHandler.hpp"
#include "Gate.hpp"
#include "KeyBinds/KeyBinds.hpp"
#include "Node.hpp"
#include "Wire.hpp"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>

inline sf::Vector2f getRandomOffset(float minOffset, float maxOffset) {
	float offsetX = minOffset + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX) / (maxOffset - minOffset));
	float offsetY = minOffset + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX) / (maxOffset - minOffset));

	return sf::Vector2f(offsetX, offsetY);
}
inline sf::Vector2f SnapToGrid(sf::Vector2f worldPos, float spacing, sf::Vector2f offset) {
	sf::Vector2f adjusted = worldPos + offset;
	adjusted.x = std::round(adjusted.x / spacing) * spacing;
	adjusted.y = std::round(adjusted.y / spacing) * spacing;
	return adjusted - offset;
}

class Simulation
{
public:
	Simulation(std::string dirPath, unsigned int tickRate);
	~Simulation();

public:
	sf::Font arial;
private:
	sf::Clock clock;
	sf::Shader shader;
	sf::Vector2f mousePos;
	sf::Vector2f lastmousePos;
	sf::Vector2f viewCenter;
	sf::Vector2f lastviewCenter;

public:
	float gridSize = 10.0f;

private:
	int thickness = 2;

public:
	float spacing = 50.0f;

private:
	const unsigned int maxtickRate;
	bool slowed = false;
	bool addingWire = false;
	bool deletedLastFrame = false;
	bool addedNodeLastFrame = false;
	bool addedGateLastFrame = false;
	bool movingObject = false;
	bool lastLeft = false;
	bool lastRight = false;
	bool lastMid = false;
	int movedNodeIdx = -1;
	int movedGateIdx = -1;
	float oldGridSize = gridSize;

private:
	// Pointer: as 'wire' references nodes and thus changing the location(when changing size) will affect the wires
	std::vector<Node*> nodes;
	std::vector<Gate*> gates;
	std::vector<Wire> wires;

public:
	void addGate(std::string gate, sf::RenderWindow& window);
	std::pair<unsigned int, unsigned int> getNodeNum();
	void addNode(sf::RenderWindow& window);
	void update(sf::RenderWindow& window);
	void draw(sf::RenderWindow& window);
	void zoom(sf::RenderWindow& window, bool followMouse);
	std::vector<Gate> getGates();
	Components getComponents();
	void deleteNode(int idx);
	void deleteGate(int idx);
	void deleteWire(int idx);
	std::string analyze();
	void deleteElement();
	void deleteAll();
};

