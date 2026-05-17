#include "Collider.h"

#include "../../Application.h"

// --------------------------------------------------------------------------------------- CIRCLE
CircleCollider::CircleCollider(float _rad, const sf::Vector2f& _pos) {
    m_centre = _pos;
    m_radius = _rad;
}

void CircleCollider::Create() {
    // Create collider visuals
#if SHOW_COLLIDER_VISUALS == 1
    m_outline = std::make_shared<sf::CircleShape>();
    m_outline->setFillColor(sf::Color::Transparent);
    m_outline->setOutlineColor(sf::Color::Red);
    m_outline->setOutlineThickness(3.0f);
    // Set circle params
    UpdateCircleShape(m_radius, m_centre);
#endif
}

// Position
void CircleCollider::SetPosition(sf::Vector2f _pos) {
    m_centre = _pos;
}
void CircleCollider::IncrementPosition(sf::Vector2f _pos) {
    m_centre += _pos;
}
sf::Vector2f CircleCollider::GetPosition() const {
    return m_centre;
}

// Rotation does nothing on circle collider (collider does not need to rotate)
void CircleCollider::SetRotation(float _rot) {
}
void CircleCollider::IncrementRotation(float _rot) {
}
float CircleCollider::GetRotation() const {
    return 0.0f;
}

// Radius
void CircleCollider::SetRadius(float _rad) {
    m_radius = _rad;
}
float CircleCollider::GetRadius() const {
    return m_radius;
}
// ---------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------- POLYGON
void PolygonCollider::Create() {
#if SHOW_COLLIDER_VISUALS == 1
    CreateVertexArray(m_verticesGlobal);
#endif
}

// Position
void PolygonCollider::SetPosition(sf::Vector2f _pos) {
    // Origin is local origin + new position
    m_originGlobal = m_originLocal + _pos;

    // Apply position
    for (int i = 0; i < static_cast<int>(m_verticesGlobal.size()); i++) {
        m_verticesGlobal[i] = m_verticesLocal[i] + _pos;
    }
    // Apply rotation
}
void PolygonCollider::IncrementPosition(sf::Vector2f _pos) {
    // Origin is global origin + position increment
    m_originGlobal += _pos;

    // Apply position
    for (auto& vert : m_verticesGlobal) {
        vert += _pos;
    }
}
sf::Vector2f PolygonCollider::GetPosition() const {
    // Return global origin
    return m_originGlobal;
}

// Rotation
void PolygonCollider::SetRotation(float _rot) {

}
void PolygonCollider::IncrementRotation(float _rot) {
    m_rotation += _rot;

    for (auto& vert : m_verticesGlobal) {
        float newVertX = m_originGlobal.x + (vert.x - m_originGlobal.x) * std::cos(_rot) - (vert.y - m_originGlobal.y) * std::sin(_rot);
        float newVertY = m_originGlobal.y + (vert.x - m_originGlobal.x) * std::sin(_rot) + (vert.y - m_originGlobal.y) * std::cos(_rot);
        
        vert = {newVertX, newVertY};
    }
}
float PolygonCollider::GetRotation() const {
    return m_rotation;
}

// Origin
void PolygonCollider::SetOrigin(const sf::Vector2f& _localPos) {
    m_originLocal = _localPos;
    m_originGlobal += _localPos;
}
sf::Vector2f PolygonCollider::GetLocalOrigin() const {
    return m_originLocal;
}
sf::Vector2f PolygonCollider::GetGlobalOrigin() const {
    return m_originGlobal;
}

// ---------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------- BOX (POLYGON)
// Creates 4 vertices, set to a real value before usage
BoxCollider::BoxCollider() {
    // Local vertices
    m_verticesLocal.emplace_back(0.0f, 0.0f);
    m_verticesLocal.emplace_back(0.0f, 0.0f);
    m_verticesLocal.emplace_back(0.0f, 0.0f);
    m_verticesLocal.emplace_back(0.0f, 0.0f);

    // Global vertices
    m_verticesGlobal = m_verticesLocal;
}

// Box declared with a size and position
BoxCollider::BoxCollider(const sf::Vector2f& _pos, const sf::Vector2f& _size) : m_size(_size) {
    // Local vertices
    m_verticesLocal.emplace_back(0.0f, 0.0f);
    m_verticesLocal.emplace_back(_size.x, 0.0f);
    m_verticesLocal.emplace_back(_size.x, _size.y);
    m_verticesLocal.emplace_back(0.0f, _size.y);

    // Global vertices
    m_verticesGlobal.emplace_back(_pos.x, _pos.y); // Top-Left
    m_verticesGlobal.emplace_back(_pos.x + _size.x, _pos.y); // Top-Right
    m_verticesGlobal.emplace_back(_pos.x + _size.x, _pos.y + _size.y); // Bottom-Right
    m_verticesGlobal.emplace_back(_pos.x, _pos.y + _size.y); // Bottom-Left
}


void BoxCollider::SetSize(sf::Vector2f _size) {
    m_size = _size;

    // Missing implementation of increasing the size of the box.
    /*
    for (auto& vert : m_verticesGlobal) {
        vert 
    }
    */
}
sf::Vector2f BoxCollider::GetSize() const {
    return m_size;
}
