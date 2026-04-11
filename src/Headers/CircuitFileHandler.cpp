#include "CircuitFileHandler.hpp"

std::pair<std::vector<unsigned int>, std::vector<unsigned int>> analyzeNodes(
    std::vector<Node*>& nodes,
    std::vector<Wire>& wires,
    std::vector<Gate*>& gates)
{
    std::pair<std::vector<unsigned int>, std::vector<unsigned int>> analyzedNodes;

    std::unordered_set<bool*> outputNodes;
    std::unordered_set<bool*> inputNodes;

    for (const Wire& wire : wires) {
        inputNodes.insert(wire.input);
        outputNodes.insert(wire.output);
    }

    for (Gate* gate : gates) {
        for (Node* node : gate->custom_inputs) {
            outputNodes.insert(reinterpret_cast<bool*>(node));
        }
        for (Node* node : gate->custom_outputs) {
            inputNodes.insert(reinterpret_cast<bool*>(node));
        }
    }

    for (size_t i = 0; i < nodes.size(); ++i) {
        bool* nodePtr = reinterpret_cast<bool*>(nodes[i]);

        if (outputNodes.find(nodePtr) != outputNodes.end() &&
            inputNodes.find(nodePtr) == inputNodes.end()) {
            analyzedNodes.second.push_back(static_cast<unsigned int>(i));
        }
        else if (inputNodes.find(nodePtr) != inputNodes.end() &&
                 outputNodes.find(nodePtr) == outputNodes.end()) {
            analyzedNodes.first.push_back(static_cast<unsigned int>(i));
        }
    }

    std::sort(analyzedNodes.first.begin(), analyzedNodes.first.end(),
        [&nodes](const unsigned int& a, const unsigned int& b) {
            return nodes[a]->position.y < nodes[b]->position.y;
        });

    std::sort(analyzedNodes.second.begin(), analyzedNodes.second.end(),
        [&nodes](const unsigned int& a, const unsigned int& b) {
            return nodes[a]->position.y < nodes[b]->position.y;
        });

    return analyzedNodes;
}

std::vector<std::string> getCustomGates(std::string path)
{
    std::vector<std::string> gates;

    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            gates.push_back(entry.path().stem().string());
        }
    }

    return gates;
}

AnalyzedCiruit analyzeCircuit(const Components& components)
{
    AnalyzedCiruit circuit;

    circuit.wireNum = components.wires.size();
    circuit.nodeNum = components.nodes.size();
    circuit.gateNum = components.gates.size();
    circuit.total = circuit.wireNum + circuit.nodeNum + circuit.gateNum;

    circuit.analyzedNodes = analyzeNodes(
        const_cast<std::vector<Node*>&>(components.nodes),
        const_cast<std::vector<Wire>&>(components.wires),
        const_cast<std::vector<Gate*>&>(components.gates)
    );

    circuit.inputNodesNum = circuit.analyzedNodes.first.size();
    circuit.outputNodesNum = circuit.analyzedNodes.second.size();

    return circuit;
}

std::string toString(const AnalyzedCiruit& circuit)
{
    std::string text;

    text += "Components num: " + std::to_string(circuit.total);
    text += "\nWires: " + std::to_string(circuit.wireNum);
    text += "\nGates: " + std::to_string(circuit.gateNum);
    text += "\nNodes: " + std::to_string(circuit.nodeNum);
    text += "\nInput Nodes num: " + std::to_string(circuit.inputNodesNum);
    text += "\nOutput Nodes num: " + std::to_string(circuit.outputNodesNum);

    text += "\nInput nodes: ";
    for (size_t i = 0; i < circuit.analyzedNodes.first.size(); ++i) {
        text += std::to_string(circuit.analyzedNodes.first[i]);
        if (i + 1 < circuit.analyzedNodes.first.size()) text += ", ";
    }

    text += "\nOutput nodes: ";
    for (size_t i = 0; i < circuit.analyzedNodes.second.size(); ++i) {
        text += std::to_string(circuit.analyzedNodes.second[i]);
        if (i + 1 < circuit.analyzedNodes.second.size()) text += ", ";
    }

    return text;
}

void reorderNodes(std::vector<Node*>& nodes)
{
    std::stable_partition(nodes.begin(), nodes.end(), [](Node* node) {
        return !node->shouldDraw;
    });
}

void load(std::string path, std::string name, Components& components, float spacing, sf::Font& arial)
{
    loadFromPath(path + name + ".json", components, spacing, arial);
}

void loadasCustom(std::string path, std::string name, Components& components, float spacing, sf::Font& arial)
{
    json data;
    std::ifstream file(path + name + ".json");

    if (file.is_open()) {
        file >> data;
        file.close();
    } else {
        std::cout << "Couldn't open: " << path + name + ".json" << std::endl;
        return;
    }

    unsigned int gateIdxOffset = components.gates.size();
    unsigned int originalNodeOffset = components.nodes.size();
    unsigned int nodeIdxOffset = components.nodes.size();

    for (json gate : data["Gates"]) {
        std::string gname = gate["Name"];

        if (gname == "NOT") components.gates.emplace_back(new NotGate(spacing, arial));
        else if (gname == "AND") components.gates.emplace_back(new AndGate(spacing, arial));
        else if (gname == "OR") components.gates.emplace_back(new OrGate(spacing, arial));
        else if (gname == "XOR") components.gates.emplace_back(new XOrGate(spacing, arial));
        else {
            loadasCustom(path, gname, components, spacing, arial);
        }

        components.gates.back()->position = {
            gate["Position"][0],
            gate["Position"][1]
        };
        components.gates.back()->shouldDraw = false;
    }

    for (json node : data["Nodes"]) {
        components.nodes.emplace_back(new Node());
        components.nodes.back()->position = {
            node["Position"][0],
            node["Position"][1]
        };
        components.nodes.back()->state = node["State"];
        components.nodes.back()->shouldDraw = false;
    }

    for (json wire : data["Wires"]) {

        int inputNodeIdx = wire.value("inputNodeIdx", -1);
        int outputNodeIdx = wire.value("outputNodeIdx", -1);
        int gateInputIdx = wire.value("gateInputIdx", -1);
        int gateOutputIdx = wire.value("gateOutputIdx", -1);
        int inputConnectorIdx = wire.value("inputConnectorIdx", -1);
        int outputConnectorIdx = wire.value("OutputConnectorIdx", -1);

        sf::Vector2f* p1 = nullptr;
        sf::Vector2f* p2 = nullptr;
        bool* input = nullptr;
        bool* output = nullptr;

        if (inputNodeIdx != -1) {
            Node* n = components.nodes[nodeIdxOffset + inputNodeIdx];
            p1 = &n->position;
            input = &n->state;
        }
        else if (gateInputIdx != -1) {
            Connector& c =
                components.gates[gateInputIdx + gateIdxOffset]
                ->outputs[inputConnectorIdx];
            p1 = &c.gloabalPosition;
            input = &c.state;
        }

        if (outputNodeIdx != -1) {
            Node* n = components.nodes[nodeIdxOffset + outputNodeIdx];
            p2 = &n->position;
            output = &n->state;
        }
        else if (gateOutputIdx != -1) {
            Connector& c =
                components.gates[gateOutputIdx + gateIdxOffset]
                ->inputs[outputConnectorIdx];
            p2 = &c.gloabalPosition;
            output = &c.state;
        }

        components.wires.emplace_back(Wire(p1, p2));
        components.wires.back().input = input;
        components.wires.back().output = output;
        components.wires.back().shouldDraw = false;
    }

    components.gates.push_back(
        new Custom(
            data["InputNodeNum"],
            data["OutputNodeNum"],
            spacing,
            data["Name"],
            sf::Color(
                data["Color"][0],
                data["Color"][1],
                data["Color"][2]
            ),
            arial
        )
    );

    int connectorIdx = 0;
    for (int i : data["InputNode"]) {
        components.gates.back()->custom_inputs[connectorIdx] =
            components.nodes[originalNodeOffset + i];
        ++connectorIdx;
    }

    connectorIdx = 0;
    for (int i : data["OutputNode"]) {
        components.gates.back()->custom_outputs[connectorIdx] =
            components.nodes[originalNodeOffset + i];
        ++connectorIdx;
    }
}

void loadFromPath(std::string path, Components& components, float spacing, sf::Font& arial)
{
    json data;
    std::ifstream file(path);

    if (file.is_open()) {
        file >> data;
        file.close();
    } else {
        std::cout << "Couldn't open: " << path << std::endl;
        return;
    }

    unsigned int gateIdxOffset = components.gates.size();
    unsigned int nodeIdxOffset = components.nodes.size();

    path = path.substr(
        0,
        path.length()
        - std::string(".json").length()
        - std::string(data["Name"]).length()
    );

    for (json gate : data["Gates"]) {
        std::string gname = gate["Name"];

        if (gname == "NOT") components.gates.emplace_back(new NotGate(spacing, arial));
        else if (gname == "AND") components.gates.emplace_back(new AndGate(spacing, arial));
        else if (gname == "OR") components.gates.emplace_back(new OrGate(spacing, arial));
        else if (gname == "XOR") components.gates.emplace_back(new XOrGate(spacing, arial));
        else {
            loadasCustom(path, gname, components, spacing, arial);
        }

        components.gates.back()->position = {
            gate["Position"][0],
            gate["Position"][1]
        };
    }

    for (json node : data["Nodes"]) {
        components.nodes.emplace_back(new Node());
        components.nodes.back()->position = {
            node["Position"][0],
            node["Position"][1]
        };
        components.nodes.back()->state = node["State"];
    }

    for (json wire : data["Wires"]) {

        int inputNodeIdx = wire.value("inputNodeIdx", -1);
        int outputNodeIdx = wire.value("outputNodeIdx", -1);
        int gateInputIdx = wire.value("gateInputIdx", -1);
        int gateOutputIdx = wire.value("gateOutputIdx", -1);
        int inputConnectorIdx = wire.value("inputConnectorIdx", -1);
        int outputConnectorIdx = wire.value("OutputConnectorIdx", -1);

        sf::Vector2f* p1 = nullptr;
        sf::Vector2f* p2 = nullptr;
        bool* input = nullptr;
        bool* output = nullptr;

        if (inputNodeIdx != -1) {
            Node* n = components.nodes[nodeIdxOffset + inputNodeIdx];
            p1 = &n->position;
            input = &n->state;
        }
        else if (gateInputIdx != -1) {
            Connector& c =
                components.gates[gateInputIdx + gateIdxOffset]
                ->outputs[inputConnectorIdx];
            p1 = &c.gloabalPosition;
            input = &c.state;
        }

        if (outputNodeIdx != -1) {
            Node* n = components.nodes[nodeIdxOffset + outputNodeIdx];
            p2 = &n->position;
            output = &n->state;
        }
        else if (gateOutputIdx != -1) {
            Connector& c =
                components.gates[gateOutputIdx + gateIdxOffset]
                ->inputs[outputConnectorIdx];
            p2 = &c.gloabalPosition;
            output = &c.state;
        }

        components.wires.emplace_back(Wire(p1, p2));
        components.wires.back().input = input;
        components.wires.back().output = output;
    }
}

void save(std::string path, std::string name, const Components& components, ImVec4 color)
{
    reorderNodes(const_cast<std::vector<Node*>&>(components.nodes));

    AnalyzedCiruit circuit = analyzeCircuit(components);

    json data = {
        {"ComponentNum", circuit.total},
        {"NodeNum", circuit.nodeNum},
        {"GateNum", circuit.gateNum},
        {"WireNum", circuit.wireNum},
        {"InputNodeNum", circuit.inputNodesNum},
        {"OutputNodeNum", circuit.outputNodesNum},
        {"InputNode", circuit.analyzedNodes.first},
        {"OutputNode", circuit.analyzedNodes.second},
        {"Name", name},
        {"Color", {color.x * 255.0f, color.y * 255.0f, color.z * 255.0f}}
    };

    json jsonNodes = json::array();
    for (Node* node : components.nodes) {
        if (!node->shouldDraw) continue;

        json jsonNode = {
            {"Position", {node->position.x, node->position.y}},
            {"State", node->state }
        };
        jsonNodes.push_back(jsonNode);
    }

    data["Nodes"] = jsonNodes;

    json jsonGates = json::array();
    for (Gate* gate : components.gates) {
        if (!gate->shouldDraw) continue;

        json jsonGate = {
            {"Position", {gate->position.x, gate->position.y}},
            {"Name", gate->name}
        };
        jsonGates.push_back(jsonGate);
    }

    data["Gates"] = jsonGates;

    struct FormattedWire {
        int inputNodeIdx = -1;
        int outputNodeIdx = -1;
        int gateInputIdx = -1;
        int inputConnectorIdx = -1;
        int gateOutputIdx = -1;
        int OutputConnectorIdx = -1;
    };

    std::vector<FormattedWire> formattedWires;

    for (const Wire& wire : components.wires) {
        if (!wire.shouldDraw) continue;

        FormattedWire currWire;

        int idx = 0;
        for (size_t i = 0; i < components.nodes.size(); ++i) {
            if (!components.nodes[i]->shouldDraw) continue;

            if (&components.nodes[i]->state == wire.input)
                currWire.inputNodeIdx = idx;
            else if (&components.nodes[i]->state == wire.output)
                currWire.outputNodeIdx = idx;

            ++idx;
        }

        for (size_t i = 0; i < components.gates.size(); ++i) {
            for (size_t j = 0; j < components.gates[i]->inputs.size(); ++j) {
                if (&components.gates[i]->inputs[j].state == wire.input) {
                    currWire.gateInputIdx = i;
                    currWire.inputConnectorIdx = j;
                }
                else if (&components.gates[i]->inputs[j].state == wire.output) {
                    currWire.gateOutputIdx = i;
                    currWire.OutputConnectorIdx = j;
                }
            }

            for (size_t j = 0; j < components.gates[i]->outputs.size(); ++j) {
                if (&components.gates[i]->outputs[j].state == wire.output) {
                    currWire.gateOutputIdx = i;
                    currWire.OutputConnectorIdx = j;
                }
                else if (&components.gates[i]->outputs[j].state == wire.input) {
                    currWire.gateInputIdx = i;
                    currWire.inputConnectorIdx = j;
                }
            }
        }

        formattedWires.push_back(currWire);
    }

    json jsonWires = json::array();
    for (const FormattedWire& wire : formattedWires) {
        json jsonWire = {
            {"inputNodeIdx", wire.inputNodeIdx},
            {"outputNodeIdx", wire.outputNodeIdx},
            {"gateInputIdx", wire.gateInputIdx},
            {"inputConnectorIdx", wire.inputConnectorIdx},
            {"gateOutputIdx", wire.gateOutputIdx},
            {"OutputConnectorIdx", wire.OutputConnectorIdx},
        };
        jsonWires.push_back(jsonWire);
    }

    data["Wires"] = jsonWires;

    std::ofstream file(path + name + ".json");
    if (file.is_open()) {
        file << data.dump(4);
        file.close();
    }
}
