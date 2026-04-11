#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Node.hpp"

class Connector {
public:
	bool state = false;
	sf::Vector2f localPosition;
	sf::Vector2f gloabalPosition;

public:
	Connector();

public:
	bool contains(sf::Vector2f point);
	void draw(sf::RenderWindow& window, sf::Vector2f origin, float spacing);
	void setFillColor(sf::Color color) { circle.setFillColor(color); }

private:
	sf::CircleShape circle;
};

class Gate
{
public:
	std::vector<Connector> inputs;
	std::vector<Connector> outputs;

public:
	// For custom gates only
	std::vector<Node*> custom_inputs;
	std::vector<Node*> custom_outputs;

public:
	sf::Vector2f position;
	bool isSelected = false;
	bool shouldDraw = true;

public:
	const std::string name;

protected:
	sf::RectangleShape rect;
	sf::Color color;
	sf::Text text;
	sf::Font& font;

public:
	Gate(std::string name, sf::Color color, sf::Font& font) : name(name), color(color), font(font) {
		text.setFont(font);
		text.setFillColor(sf::Color::Black);
		text.setString(name);
	};

public:
	void drawConnectors(sf::RenderWindow& window, float spacing);
	void draw(sf::RenderWindow& window, float spacing);
	void drawText(sf::RenderWindow& window, float spacing);
	void setuniforms(sf::Shader& shader, std::string prefix, float spacing);
	bool contains(sf::Vector2f point);
	virtual void update() {};
	virtual void resize(float spacing) {};
};

class NotGate : public Gate {
public:
	NotGate(float spacing, sf::Font& font) : Gate("NOT", sf::Color(219, 22, 47), font) {
		inputs.resize(1);
		outputs.resize(1);

		resize(spacing);
	};

public:
	virtual void update() {
		outputs[0].state = !inputs[0].state;
	}
	virtual void resize(float spacing) {
		spacing *= 1.5f;

		rect.setSize(sf::Vector2f(spacing, spacing / 2.0f));
		rect.setOrigin(rect.getSize() / 2.0f);
		rect.setOutlineThickness(2.0f);
		rect.setOutlineColor(sf::Color::Black);

		inputs[0].localPosition.x = -rect.getSize().y;
		outputs[0].localPosition.x = rect.getSize().y;
	}
};

class AndGate : public Gate {
public:
	AndGate(float spacing, sf::Font& font) : Gate("AND", sf::Color(2, 103, 193), font) {
		inputs.resize(2);
		outputs.resize(1);

		resize(spacing);
	};

public:
	virtual void update() {
		outputs[0].state = inputs[0].state && inputs[1].state;
	}

	virtual void resize(float spacing) {
		spacing *= 1.5f;

		rect.setSize(sf::Vector2f(spacing, spacing / 2.0f));
		rect.setOrigin(rect.getSize() / 2.0f);
		rect.setOutlineThickness(2.0f);
		rect.setOutlineColor(sf::Color::Black);

		inputs[0].localPosition.x = -rect.getSize().y;
		inputs[1].localPosition.x = -rect.getSize().y;
		outputs[0].localPosition.x = rect.getSize().y;

		inputs[0].localPosition.y = -rect.getSize().y / 2.0f + spacing / 8.5f;
		inputs[1].localPosition.y = rect.getSize().y / 2.0f - spacing / 8.5f;
	}
};

class OrGate : public Gate {
public:
	OrGate(float spacing, sf::Font& font) : Gate("OR", sf::Color(255, 169, 41), font) {
		inputs.resize(2);
		outputs.resize(1);

		resize(spacing);
	};

public:
	virtual void update() {
		outputs[0].state = inputs[0].state || inputs[1].state;
	}

	virtual void resize(float spacing) {
		spacing *= 1.5f;

		rect.setSize(sf::Vector2f(spacing, spacing / 2.0f));
		rect.setOrigin(rect.getSize() / 2.0f);
		rect.setOutlineThickness(2.0f);
		rect.setOutlineColor(sf::Color::Black);

		inputs[0].localPosition.x = -rect.getSize().y;
		inputs[1].localPosition.x = -rect.getSize().y;
		outputs[0].localPosition.x = rect.getSize().y;

		inputs[0].localPosition.y = -rect.getSize().y / 2.0f + spacing / 8.5f;
		inputs[1].localPosition.y = rect.getSize().y / 2.0f - spacing / 8.5f;
	}
};

class XOrGate : public Gate {
public:
	XOrGate(float spacing, sf::Font& font) : Gate("XOR", sf::Color(128, 219, 41), font) {
		inputs.resize(2);
		outputs.resize(1);

		resize(spacing);
	};

public:
	virtual void update() {
		outputs[0].state = inputs[0].state ^ inputs[1].state;
	}

	virtual void resize(float spacing) {
		spacing *= 1.5f;

		rect.setSize(sf::Vector2f(spacing, spacing / 2.0f));
		rect.setOrigin(rect.getSize() / 2.0f);
		rect.setOutlineThickness(2.0f);
		rect.setOutlineColor(sf::Color::Black);

		inputs[0].localPosition.x = -rect.getSize().y;
		inputs[1].localPosition.x = -rect.getSize().y;
		outputs[0].localPosition.x = rect.getSize().y;

		inputs[0].localPosition.y = -rect.getSize().y / 2.0f + spacing / 8.5f;
		inputs[1].localPosition.y = rect.getSize().y / 2.0f - spacing / 8.5f;
	}
};

class HexDisplay: public Gate {
public:
	HexDisplay(float spacing, sf::Font& font) : Gate("HEX-DISPLAY", sf::Color(128, 128, 128), font) {
		inputs.resize(4);
		outputs.resize(4);

		resize(spacing);
	};

public:
	virtual void update() {
		for (int i = 0; i < 4; ++i) {
			outputs[i].state = inputs[i].state;
		}
	}

	virtual void resize(float spacing) {
		spacing *= 1.5f;

		rect.setSize(sf::Vector2f(spacing, spacing / 2.0f));
		rect.setOrigin(rect.getSize() / 2.0f);
		rect.setOutlineThickness(2.0f);
		rect.setOutlineColor(sf::Color::Black);

		inputs[0].localPosition.x = -rect.getSize().y;
		inputs[1].localPosition.x = -rect.getSize().y;
		outputs[0].localPosition.x = rect.getSize().y;

		inputs[0].localPosition.y = -rect.getSize().y / 2.0f + spacing / 8.5f;
		inputs[1].localPosition.y = rect.getSize().y / 2.0f - spacing / 8.5f;
	}
};

class Custom : public Gate {
public:
	Custom(int inputNum, int outputNum, float spacing, std::string name, sf::Color color, sf::Font& font) : Gate(name, color, font) {
		inputs.resize(inputNum);
		outputs.resize(outputNum);

		custom_inputs.resize(inputNum);
		custom_outputs.resize(outputNum);

		text.setCharacterSize(spacing / 2.0f);

		resize(spacing);
	};

public:
	virtual void update() {
		for (int i = 0; i < inputs.size(); ++i) {
			custom_inputs[i]->state = inputs[i].state;
		}
		for (int i = 0; i < outputs.size(); ++i) {
			outputs[i].state = custom_outputs[i]->state;
		}
	}
	virtual void resize(float spacing) {
		if (inputs.size() == 2 && outputs.size() <= 2) {
			spacing *= 1.5f;

			rect.setSize(sf::Vector2f(spacing, spacing / 2.0f));
			rect.setOrigin(rect.getSize() / 2.0f);
			rect.setOutlineThickness(2.0f);
			rect.setOutlineColor(sf::Color::Black);

			inputs[0].localPosition.x = -rect.getSize().y;
			inputs[1].localPosition.x = -rect.getSize().y;
			outputs[0].localPosition.x = rect.getSize().y;

			inputs[0].localPosition.y = -rect.getSize().y / 2.0f + spacing / 8.5f;
			inputs[1].localPosition.y = rect.getSize().y / 2.0f - spacing / 8.5f;

			return;
		}

		// Determine the maximum count between inputs and outputs
		size_t maxConnections = std::max(inputs.size(), outputs.size());
		float minHeight = spacing / 2.0f;  // Minimum height for small gates
		float padding = spacing / 3.0f;    // Extra spacing for better readability
		float dynamicHeight = std::max(minHeight, maxConnections * (spacing / 3.0f) + padding); // Increase spacing

		text.setCharacterSize(spacing / 2.0f);

		// Set the gate size dynamically
		rect.setSize(sf::Vector2f(text.getGlobalBounds().width + spacing / 3.0f, dynamicHeight));
		rect.setOrigin(rect.getSize() / 2.0f);
		rect.setOutlineThickness(2.0f);
		rect.setOutlineColor(sf::Color::Black);

		float rectHeight = rect.getSize().y;

		// Dynamically position inputs with extra spacing
		for (size_t i = 0; i < inputs.size(); ++i) {
			float step = rectHeight / (inputs.size() + 1.5f); // Increase spacing factor
			inputs[i].localPosition.x = -rect.getSize().x / 2.0f; // Left side
			inputs[i].localPosition.y = -rectHeight / 2.0f + (i + 1) * step;
		}

		// Dynamically position outputs with extra spacing
		for (size_t i = 0; i < outputs.size(); ++i) {
			float step = rectHeight / (outputs.size() + 1.5f); // Increase spacing factor
			outputs[i].localPosition.x = rect.getSize().x / 2.0f; // Right side
			outputs[i].localPosition.y = -rectHeight / 2.0f + (i + 1) * step;
		}

	}
};