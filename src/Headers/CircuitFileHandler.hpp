#pragma once
#include <unordered_set>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "nlohmann/json.hpp"
#include "imgui.h"
#include "Node.hpp"
#include "Gate.hpp"
#include "Wire.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

struct Components {
	// Pointer: as 'wire' references nodes and thus changing the location(when changing size) will affect the wires
	std::vector<Node*>& nodes;
	std::vector<Gate*>& gates;
	std::vector<Wire>& wires;
};

struct AnalyzedCiruit {
	int wireNum = 0;
	int nodeNum = 0;
	int gateNum = 0;
	int total = 0;
	int inputNodesNum = 0;
	int outputNodesNum = 0;
	std::pair<std::vector<unsigned int>, std::vector<unsigned int>> analyzedNodes;
};

std::pair<std::vector<unsigned int>, std::vector<unsigned int>> analyzeNodes(std::vector<Node*>& nodes, std::vector<Wire>& wires, std::vector<Gate*>& gates);
void loadasCustom(std::string path, std::string name, Components& components, float spacing, sf::Font& arial);
void load(std::string path, std::string name, Components& components, float spacing, sf::Font& arial);
void loadFromPath(std::string path, Components& components, float spacing, sf::Font& arial);
void save(std::string path, std::string name, const Components& components, ImVec4 color);
std::vector<std::string> getCustomGates(std::string path);
AnalyzedCiruit analyzeCircuit(const Components& components);
std::string toString(const AnalyzedCiruit& circuit);
void reorderNodes(std::vector<Node*>& nodes);
