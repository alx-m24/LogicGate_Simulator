#include "Wire/BezierCurve.hpp"

#include <cmath>
#include <SFML/Graphics.hpp>

void BezierCurve::SetResolution(uint32_t resolution) {
    m_resolution = resolution;
    GenerateCurve();
}

void BezierCurve::SetColor(sf::Color startColor, sf::Color endColor, float progress) {
    m_startColor = startColor;
    m_endColor = endColor;
    m_progress = progress;
    GenerateCurve();
}

void BezierCurve::SetPoints(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f c1, sf::Vector2f c2) {
    m_p1 = p1;
    m_p2 = p2;
    m_c1 = c1;
    m_c2 = c2;
    GenerateCurve();
}

void BezierCurve::SetThickness(float thickness) {
    m_thickness = thickness;
    GenerateCurve();
}

void BezierCurve::GenerateCurve() {
    m_points.clear();
    m_points.resize(m_resolution * 2);

    for (uint32_t i = 0; i < m_resolution; ++i) {
        float t = static_cast<float>(i) / (m_resolution - 1);
        float u = 1.0f - t;
    
        sf::Vector2f pos =
            (u * u * u) * m_p1 +
            (3.0f * u * u * t) * m_c1 +
            (3.0f * u * t * t) * m_c2 +
            (t * t * t) * m_p2;
    
        // --- Compute tangent ---
        float dt = 0.001f;
        float t2 = std::min(t + dt, 1.0f);
        float u2 = 1.0f - t2;
    
        sf::Vector2f next =
            (u2 * u2 * u2) * m_p1 +
            (3.0f * u2 * u2 * t2) * m_c1 +
            (3.0f * u2 * t2 * t2) * m_c2 +
            (t2 * t2 * t2) * m_p2;
    
        sf::Vector2f tangent = next - pos;
    
        // Normalize
        float len = std::sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
        if (len != 0)
            tangent /= len;
    
        // Perpendicular (normal)
        sf::Vector2f normal(-tangent.y, tangent.x);
    
        float half = m_thickness * 0.5f;
    
        sf::Vector2f left  = pos - normal * half;
        sf::Vector2f right = pos + normal * half;
    
        // Color logic 
        float color_t = static_cast<float>(i) / (m_resolution - 1);
        sf::Color color = (color_t <= m_progress) ? m_endColor : m_startColor;
    
        // Fill triangle strip
        m_points[i * 2].position = left;
        m_points[i * 2].color = color;
    
        m_points[i * 2 + 1].position = right;
        m_points[i * 2 + 1].color = color;
    }
}

void BezierCurve::Draw(sf::RenderTarget& target) const {
    target.draw(m_points);
}
