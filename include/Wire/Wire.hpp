#pragma once

#include <array>
#include <variant>
#include <type_traits>

#include "Globals.hpp"
#include "Node/Node.hpp"
#include "BezierCurve.hpp"
#include "Storage/Storage.hpp"

class Wire {
    private:
        float m_progress{};
        bool m_pastValue{};

        float m_lastViewportScale = g_ViewportScale;

    private:
        std::variant<Handle<Node>, int> m_input;
        std::variant<Handle<Node>, int> m_output;

        BezierCurve m_curve{};

    public:
        Wire() : m_curve(30.0f, 5.0f * g_ViewportScale) {};

    public:
        template<typename T>
        requires std::is_same_v<Node, T> || std::is_same_v<int, T>
        void setInput(Handle<T> input) {
            m_input = input;
        }

        template<typename T>
        requires std::is_same_v<Node, T> || std::is_same_v<int, T>
        void setOutput(Handle<T> output) {
            m_output = output;
        }

    public:
        void Update(Storage<Node>& nodes, float dt) {
            bool value{};
            if (std::holds_alternative<Handle<Node>>(m_input)) {
                value = nodes.get(std::get<Handle<Node>>(m_input)).getState();
            }

            if (value != m_pastValue) {
                m_progress = 0.0f;
            }
            if (value) {
                m_progress = std::min(1.0f, m_progress + VISUAL_PROGRESS_RATE * dt);
            }

            if (std::holds_alternative<Handle<Node>>(m_output)) {
                nodes.get(std::get<Handle<Node>>(m_output)).setState(m_progress >= 0.99f);
            }

            m_pastValue = value;
        }

        void Render(sf::RenderTarget& target, Storage<Node>& nodes) {
            if (m_lastViewportScale != g_ViewportScale) {
                m_curve.SetThickness(5.0f * g_ViewportScale);
                m_lastViewportScale = g_ViewportScale;
            }

            std::array<sf::Vector2f, 2> nodePositions{};

            if (std::holds_alternative<Handle<Node>>(m_input)) {
                nodePositions[0] = nodes.get(std::get<Handle<Node>>(m_input)).getPosition() + g_worldOffset;
            }
            else {
                // TODO: GATE
            }
            if (std::holds_alternative<Handle<Node>>(m_output)) {
                nodePositions[1] = nodes.get(std::get<Handle<Node>>(m_output)).getPosition() + g_worldOffset;
            }
            else {
                // TODO: GATE
            }

            uint32_t i = 0;
            m_curve.SetPoints(
                // P1
                { nodePositions[i].x, nodePositions[i].y },
                // P2
                { nodePositions[i + 1].x, nodePositions[i + 1].y },
                // C1
                { nodePositions[i + 1].x, nodePositions[i].y },
                // C2
                { nodePositions[i].x, nodePositions[i + 1].y }
                );
            m_curve.SetColor(sf::Color::White, sf::Color::Red, m_progress);
        
            m_curve.Draw(target);
        }
};
