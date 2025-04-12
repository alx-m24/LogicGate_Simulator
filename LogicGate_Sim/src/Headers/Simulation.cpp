#include "Simulation.hpp"

Simulation::Simulation(std::string dirPath, unsigned int maxtickRate) : maxtickRate(maxtickRate)
{
	shader.loadFromFile(dirPath + "\\Shaders\\mainShader.frag", sf::Shader::Fragment);
	shader.setUniform("thickness", thickness);

	if (!arial.loadFromFile("C:\\Windows\\Fonts\\Arial.ttf")) std::cout << "Could not load font" << std::endl;

	clock.restart();
	
	// Seed the random number generator
	std::srand(static_cast<unsigned>(std::time(0)));
}

Simulation::~Simulation()
{
	for (Node* node : nodes) {
		delete node;
	}
	for (Gate* gate : gates) {
		delete gate;
	}
}

void Simulation::update(sf::RenderWindow& window)
{
#pragma region KeyBinds
	if (ADD_NODE && !addedNodeLastFrame) {
		nodes.emplace_back(new Node());
		nodes.back()->state = false;
		nodes.back()->position = (sf::Vector2f(window.getSize()) / 2.0f) + getRandomOffset(-50.0f, 50.0f);
	}

	slowed = SPACE;
	if (slowed) {
		sf::Clock delay;
		while (delay.getElapsedTime().asSeconds() < 0.75f);
	}

	deleteElement();

	deletedLastFrame = DELETE;
	addedNodeLastFrame = ADD_NODE;
	addedGateLastFrame = ADD_GATE;
#pragma endregion

#pragma region Mouse

#pragma region Position
	lastmousePos = mousePos;
	mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
#pragma endregion

#pragma region Left mouse btn
	if (LEFTMOUSE && !lastLeft) {
		for (int i = nodes.size() - 1; i >= 0 && !movingObject; --i) {
			if (nodes[i]->contains(mousePos) && nodes[i]->shouldDraw) {
				movingObject = true;
				movedNodeIdx = i;
				break;
			}
		}
		for (int i = gates.size() - 1; i >= 0 && !movingObject; --i) {
			if (gates[i]->contains(mousePos) && gates[i]->shouldDraw) {
				movingObject = true;
				movedGateIdx = i;
				break;
			}
		}

	}
	if (lastLeft && !LEFTMOUSE) {
		movingObject = false;
		movedNodeIdx = -1;
		movedGateIdx = -1;
	}

	lastLeft = LEFTMOUSE;

#pragma endregion

#pragma region Right Mouse btn
	if (RIGHTMOUSE) {
		for (int i = nodes.size() - 1; i >= 0 && !movingObject; --i) {
			if (nodes[i]->contains(mousePos) && nodes[i]->shouldDraw) {
				nodes[i]->selected = true;
				break;
			}
		}
		viewCenter += sf::Vector2f(lastmousePos - mousePos);
		zoom(window, false);
	}
	// Just released the right mouse btn
	else if (lastRight) {
		for (int i = 0; i < nodes.size() && !movingObject; ++i) {
			if (nodes[i]->contains(mousePos) && nodes[i]->shouldDraw) {
				nodes[i]->state = !nodes[i]->state;
				break;
			}
		}
	}

	lastRight = RIGHTMOUSE;
#pragma endregion

#pragma region Middle Mouse btn
	if (!addingWire) {
		if (MIDDLEMOUSE) {
			for (int i = 0; i < nodes.size() && !movingObject; ++i) {
				if (nodes[i]->contains(mousePos) && nodes[i]->shouldDraw) {
					nodes[i]->selected = true;
					break;
				}
			}
			for (int i = 0; i < gates.size() && !movingObject; ++i) {
				if (!gates[i]->shouldDraw) continue;
				for (Connector& connector : gates[i]->inputs) {
					if (connector.contains(mousePos)) {
						gates[i]->isSelected = true;
						break;
					}
				}
				for (Connector& connector : gates[i]->outputs) {
					if (connector.contains(mousePos)) {
						gates[i]->isSelected = true;
						break;
					}
				}
			}
		}
		// Just released the middle mouse btn
		else if (lastMid) {
			for (int i = 0; i < nodes.size() && !movingObject; ++i) {
				if (nodes[i]->contains(mousePos) && nodes[i]->shouldDraw) {
					addingWire = true;

					wires.emplace_back(Wire(&(nodes[i]->position), &mousePos));

					wires.back().input = &nodes[i]->state;
					break;
				}
			}
			for (int i = 0; i < gates.size() && !movingObject; ++i) {
				if (!gates[i]->shouldDraw) continue;
				for (Connector& connector : gates[i]->inputs) {
					if (connector.contains(mousePos)) {
						addingWire = true;

						wires.emplace_back(Wire(&connector.gloabalPosition, &mousePos));

						wires.back().input = &connector.state;
						break;
					}
				}
				for (Connector& connector : gates[i]->outputs) {
					if (connector.contains(mousePos)) {
						addingWire = true;

						wires.emplace_back(Wire(&connector.gloabalPosition, &mousePos));

						wires.back().input = &connector.state;
						break;
					}
				}
			}
		}
	}
	else {
		if (MIDDLEMOUSE) {
			for (int i = 0; i < nodes.size() && !movingObject; ++i) {
				if (nodes[i]->contains(mousePos) && nodes[i]->shouldDraw) {
					nodes[i]->selected = true;
					break;
				}
			}
			for (int i = 0; i < gates.size() && !movingObject; ++i) {
				if (!gates[i]->shouldDraw) continue;
				for (Connector connector : gates[i]->inputs) {
					if (connector.contains(mousePos)) {
						gates[i]->isSelected = true;
						break;
					}
				}
				for (Connector connector : gates[i]->outputs) {
					if (connector.contains(mousePos)) {
						gates[i]->isSelected = true;
						break;
					}
				}
			}
		}
		// Just released the middle mouse btn
		else if (lastMid) {
			addingWire = false;
			bool addedWire = false;
			for (int i = 0; i < nodes.size() && !movingObject; ++i) {
				if (nodes[i]->contains(mousePos) && nodes[i]->shouldDraw) {
					wires.back().p2 = &(nodes[i]->position);

					void* tempPtr = &(*nodes[i]);
					wires.back().output = static_cast<bool*>(tempPtr);

					addedWire = true;
					break;
				}
			}
			for (int i = 0; i < gates.size() && !movingObject; ++i) {
				if (!gates[i]->shouldDraw) continue;
				for (Connector& connector : gates[i]->inputs) {
					if (connector.contains(mousePos)) {
						wires.back().p2 = &connector.gloabalPosition;
						wires.back().output = &connector.state;

						addedWire = true;
						break;
					}
				}
				for (Connector& connector : gates[i]->outputs) {
					if (connector.contains(mousePos)) {
						wires.back().p2 = &connector.gloabalPosition;
						wires.back().output = &connector.state;

						addedWire = true;
						break;
					}
				}
			}
			if (!addedWire) {
				wires.pop_back();
			}
		}
	}

	lastMid = MIDDLEMOUSE;
#pragma endregion

#pragma endregion	

#pragma region Move node
	if (movedNodeIdx != -1) {
		nodes[movedNodeIdx]->position = mousePos;
		nodes[movedNodeIdx]->selected = true;
		if (SHIFT) {
			// Clamping position
			nodes[movedNodeIdx]->position = SnapToGrid(nodes[movedNodeIdx]->position, spacing, viewCenter + sf::Vector2f(spacing, spacing) / 2.0f);
		}
	}
#pragma endregion

#pragma region Move gate
	if (movedGateIdx != -1) {
		gates[movedGateIdx]->position = mousePos;
		gates[movedGateIdx]->isSelected = true;
		if (SHIFT) {
			// Clamping position
			gates[movedGateIdx]->position = SnapToGrid(gates[movedGateIdx]->position, spacing, viewCenter + (sf::Vector2f(spacing, spacing) / 2.0f));
		}
	}
#pragma endregion

#pragma region Physics frames
	if (slowed) {
		for (Wire& wire : wires) wire.update();
		for (Gate* gate : gates) {
			bool shouldUpdate = static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX) / (1.0f)) > 0.25f;
			if (shouldUpdate) gate->update();
		}
	}
	else {
		for (unsigned int i = 0; i < std::min(static_cast<unsigned int>(gates.size() + 1), maxtickRate); ++i) {
			for (Wire& wire : wires) wire.update();
			for (Gate* gate : gates) {
				bool shouldUpdate = static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX) / (1.0f)) > 0.25f;
				if (shouldUpdate) gate->update();
			}
		}
	}
#pragma endregion
}

void Simulation::draw(sf::RenderWindow& window)
{
	sf::Vector2f windowSize = sf::Vector2f(window.getSize());

#pragma region Background
	sf::RectangleShape background;
	background.setSize(windowSize);
	background.setPosition(0.0f, 0.0f);

	shader.setUniform("resolution", windowSize);
	shader.setUniform("time", clock.getElapsedTime().asSeconds());

	int nodeNum = 0;
	for (Node* node : nodes) {
		if (!node->shouldDraw) continue;

		std::string name = "nodes[" + std::to_string(nodeNum) + "].";
		shader.setUniform(name + "state", node->state);
		shader.setUniform(name + "position", node->position);
		shader.setUniform(name + "activeColor", sf::Vector3f(static_cast<sf::Uint8>(node->activeColor.r), static_cast<sf::Uint8>(node->activeColor.g), static_cast<sf::Uint8>(node->activeColor.b)));

		++nodeNum;
	}
	shader.setUniform("nodeNum", nodeNum);

	int wireNum = 0;
	for (const Wire& wire : wires) {
		if (!wire.shouldDraw) continue;

		std::string name = "wires[" + std::to_string(wireNum) + "].";
		shader.setUniform(name + "state", wire.getState());
		shader.setUniform(name + "p1", *wire.p1);
		shader.setUniform(name + "p2", *wire.p2);
		shader.setUniform(name + "activeColor", sf::Vector3f(static_cast<sf::Uint8>(wire.activeColor.r), static_cast<sf::Uint8>(wire.activeColor.g), static_cast<sf::Uint8>(wire.activeColor.b)));

		++wireNum;
	}
	shader.setUniform("wireNum", wireNum);

	int gateNum = 0;
	for (Gate* gate : gates) {
		if (!gate->shouldDraw) continue;

		std::string name = "gates[" + std::to_string(gateNum) + "].";
		gate->setuniforms(shader, name, spacing);

		++gateNum;
	}
	shader.setUniform("gateNum", gateNum);

	window.draw(background, &shader);
#pragma endregion

	for (Gate* gate : gates) {
		gate->draw(window, spacing);
	}
	for (Node* node : nodes) {
		node->draw(window, spacing / 2.0f);
	}
}

void Simulation::zoom(sf::RenderWindow& window, bool followMouse)
{
#pragma region Spacing
	spacing = std::min(window.getSize().x, window.getSize().y) / gridSize;

	if (gridSize > 20.0f) thickness = 1;
	else thickness = 2;
#pragma endregion

	if (followMouse) {
		viewCenter += sf::Vector2f(mousePos);

		for (Node* node : nodes) {
			node->position = (node->position - (viewCenter - lastviewCenter)) * oldGridSize / gridSize + (viewCenter - lastviewCenter);
		}

		for (Gate* gate : gates) {
			gate->position = (gate->position - (viewCenter - lastviewCenter)) * oldGridSize / gridSize + (viewCenter - lastviewCenter);
			gate->resize(spacing);
		}
	}
	else {
		for (Node* node : nodes) {
			node->position = (node->position - (viewCenter - lastviewCenter)) * oldGridSize / gridSize;
		}

		for (Gate* gate : gates) {
			gate->position = (gate->position - (viewCenter - lastviewCenter)) * oldGridSize / gridSize;
			gate->resize(spacing);
		}
	}

	shader.setUniform("viewCenter", viewCenter);
	shader.setUniform("spacing", spacing);

	oldGridSize = gridSize;
	lastviewCenter = viewCenter;
}

void Simulation::addGate(std::string gate, sf::RenderWindow& window)
{
	if (gate == "NOT") gates.emplace_back(new NotGate(spacing, arial));
	if (gate == "AND") gates.emplace_back(new AndGate(spacing, arial));
	if (gate == "OR") gates.emplace_back(new OrGate(spacing, arial));
	if (gate == "XOR") gates.emplace_back(new XOrGate(spacing, arial));

	gates.back()->position = (sf::Vector2f(window.getSize()) / 2.0f) + getRandomOffset(-50.0f, 50.0f);
}

void Simulation::addNode(sf::RenderWindow& window)
{
	nodes.emplace_back(new Node());
	nodes.back()->state = false;
	nodes.back()->position = (sf::Vector2f(window.getSize()) / 2.0f) + getRandomOffset(-50.0f, 50.0f);
}

std::vector<Gate> Simulation::getGates()
{
	std::vector<Gate> gates;

	gates.emplace_back(NotGate(spacing, arial));
	gates.emplace_back(AndGate(spacing, arial));
	gates.emplace_back(OrGate(spacing, arial));
	gates.emplace_back(XOrGate(spacing, arial));

	return gates;
}

std::string Simulation::analyze()
{
	return toString(analyzeCircuit(getComponents()));
}

void Simulation::deleteElement()
{
	if (DELETE) {
		for (int i = nodes.size() - 1; i >= 0; --i) {
			if (nodes[i]->contains(mousePos) && nodes[i]->shouldDraw) {
				nodes[i]->selected = true;
				break;
			}
		}
		for (int i = gates.size() - 1; i >= 0; --i) {
			if (gates[i]->contains(mousePos) && gates[i]->shouldDraw) {
				gates[i]->isSelected = true;
				break;
			}
		}
	}
	// Just released the delete btn
	else if (deletedLastFrame) {
		bool deleted = false;

		for (int i = nodes.size() - 1; i >= 0; --i) {
			if (nodes[i]->contains(mousePos) && nodes[i]->shouldDraw) {
				deleteNode(i);
				deleted = true;
				break;
			}
		}

		if (deleted) return;

		for (int i = gates.size() - 1; i >= 0; --i) {
			if (gates[i]->contains(mousePos) && gates[i]->shouldDraw) {
				deleteGate(i);
				deleted = true;
				break;
			}
		}

		if (deleted) return;

		for (int i = wires.size() - 1; i >= 0; --i) {
			if (wires[i].contains(mousePos) && wires[i].shouldDraw) {
				deleteWire(i);
				break;
			}
		}
	}
}

std::pair<unsigned int, unsigned int> Simulation::getNodeNum()
{
	AnalyzedCiruit circuit = analyzeCircuit(getComponents());
	return std::pair<unsigned int, unsigned int>(circuit.inputNodesNum, circuit.outputNodesNum);
}

Components Simulation::getComponents()
{
	return Components{ nodes, gates, wires };
}

void Simulation::deleteNode(int idx)
{
	std::vector<int> wiresToDelete;
	for (int i = 0; i < wires.size(); ++i) {
		if (!wires[i].shouldDraw) continue;

		if (wires[i].input == &nodes[idx]->state || wires[i].output == &nodes[idx]->state) {
			wiresToDelete.push_back(i);
		}
	}

	delete nodes[idx];
	nodes.erase(nodes.begin() + idx);

	// Sorting in order to delete wires from back to front
	std::sort(wiresToDelete.begin(), wiresToDelete.end(), std::greater<int>());
	for (const int& i : wiresToDelete) {
		wires.erase(wires.begin() + i);
	}
}

void Simulation::deleteGate(int idx)
{
	std::vector<int> wiresToDelete;
	for (int i = 0; i < wires.size(); ++i) {
		if (!wires[i].shouldDraw) continue;

		for (const Connector& input : gates[idx]->inputs) {
			if (wires[i].input == &input.state || wires[i].output == &input.state) wiresToDelete.push_back(i);
		}
		for (const Connector& output : gates[idx]->outputs) {
			if (wires[i].input == &output.state || wires[i].output == &output.state) wiresToDelete.push_back(i);
		}
	}

	delete gates[idx];
	gates.erase(gates.begin() + idx);

	// Sorting in order to delete wires from back to front
	std::sort(wiresToDelete.begin(), wiresToDelete.end(), std::greater<int>());
	for (const int& i : wiresToDelete) {
		wires.erase(wires.begin() + i);
	}
}

void Simulation::deleteWire(int idx)
{
	wires.erase(wires.begin() + idx);
}

void Simulation::deleteAll()
{
	for (Node* node : nodes) {
		delete node;
	}
	for (Gate* gate : gates) {
		delete gate;
	}

	nodes.clear();
	gates.clear();
	wires.clear();
}