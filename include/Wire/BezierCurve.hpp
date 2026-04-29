#pragma once

#include <SFML/Graphics.hpp>

class BezierCurve {
    private:
        uint32_t m_resolution = 30u;

        float m_thickness = 1.0f;

    private:
        sf::VertexArray m_points;

        sf::Vector2f m_p1{};
        sf::Vector2f m_p2;

        sf::Vector2f m_c1{};
        sf::Vector2f m_c2{};

        sf::Color m_startColor{};
        sf::Color m_endColor{};
        float m_progress = 1.0f;

    public:
        BezierCurve() : m_points(sf::VertexArray(sf::PrimitiveType::TrianglesStrip)) {}
        BezierCurve(uint32_t resolution) : m_resolution(resolution), m_points(sf::VertexArray(sf::PrimitiveType::TrianglesStrip, resolution)) {}
        BezierCurve(uint32_t resolution, float thickness) : m_resolution(resolution), m_thickness(thickness), m_points(sf::VertexArray(sf::PrimitiveType::TrianglesStrip, resolution)) {}

    private:
        void GenerateCurve();


    public:
        void SetThickness(float thickness);
        void SetResolution(uint32_t resolution);
        void SetColor(sf::Color startColor, sf::Color endColor, float progress = 1.0f);
        void SetPoints(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f c1, sf::Vector2f c2);
        
    public:
        void Draw(sf::RenderTarget& target) const;
};
