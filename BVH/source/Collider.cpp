#include "Collider.h"

#include <valarray>

#include "Application.h"
#include "Physics/SATCollision.h"

// Base collider --------------------------------------------------------------------------------------------------------------
void Collider::SetPosition(sf::Vector2f _pos)
{
    m_position = _pos;
}

void Collider::IncrementPosition(sf::Vector2f _pos)
{
    m_position += _pos;
}

sf::Vector2f Collider::GetPosition() const
{
    return m_position;
}

void Collider::SetOrigin(sf::Vector2f origin)
{
    m_origin = origin;
}

sf::Vector2f Collider::GetOrigin()
{
    return m_origin;
}

void Collider::SetVertexCount(size_t vertexCount)
{
    m_vertexCount = vertexCount;
}

// Circle Collider --------------------------------------------------------------------------------------------------------------
void CircleCollider::CreateCollider() {

}

bool CircleCollider::CollideWith(Collider* otherCollider) const {
    return false;
}

bool CircleCollider::CollideWith(PolygonCollider* otherCollider) const { // Circle, Polygon
    return false;
}

bool CircleCollider::CollideWith(CircleCollider* otherCollider) const { // Circle, Circle
    return false;
}

// Polygon Collider --------------------------------------------------------------------------------------------------------------
void PolygonCollider::CreateCollider() {
    m_vertArray.setPrimitiveType(sf::Quads);
    m_vertArray.resize(m_vertices.size());
    UpdateVertexArray();

    // Set the normal vert array
    m_vertArrayNormal = m_vertArray;
}

bool PolygonCollider::CollideWith(Collider* otherCollider) const {
    return false;
}

bool PolygonCollider::CollideWith(PolygonCollider* otherCollider) const {
    return Physics::CollisionDetection::PolygonOnPolygonSATCollision(this, otherCollider);
}


bool PolygonCollider::CollideWith(CircleCollider* otherCollider) const {
    return false;
}

void PolygonCollider::SetPosition(sf::Vector2f _pos) {
    // I need to simplify this to work using local rect + global rect, this is the problem with working with
    // a code base that was written a few months ago and needs reworking

    // Get local vertex positions
    for (auto& vec : m_vertices) {
        vec -= m_position;
    }

    Collider::SetPosition(_pos);

    // Add to all vertex positions
    for (auto& vec : m_vertices) {
        vec += _pos;
    }
    // Set to origin
    m_origin = _pos;
}

void PolygonCollider::IncrementPosition(sf::Vector2f _pos) {
    Collider::IncrementPosition(_pos);

    // Add to all vertex positions
    for (auto& vec : m_vertices) {
        vec += _pos;
    }
    // Add to origin
    m_origin += _pos;

    UpdateVertexArray();
}

void PolygonCollider::IncrementRotation(float _rot) {
    m_rotation += _rot;

    for (auto& vert : m_vertices) {
        sf::Vector2f temp = vert - m_origin;

        vert.x = (temp.x * std::cos(_rot)) - (temp.y * std::sin(_rot));
        vert.y = (temp.y * std::cos(_rot)) + (temp.x * std::sin(_rot));
    }
    UpdateVertexArray();
}

void PolygonCollider::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_vertArray, states);
}

void BoxCollider::SetSize(sf::Vector2f _size) {
    // Keep top-left the same

    // Top-Right
    m_vertices[1] = { m_vertices[1].x + _size.x, m_vertices[1].y };
    // Bottom-Right
    m_vertices[2] = { m_vertices[2].x + _size.x, m_vertices[2].y + _size.y };
    // Bottom-Left
    m_vertices[3] = { m_vertices[3].x, m_vertices[3].y + _size.y };

    UpdateVertexArray();
}

sf::Vector2f BoxCollider::GetSize() const {
    return m_size;
}

void BoxCollider::SetPosition(sf::Vector2f position) {

}

void BoxCollider::IncrementPosition(sf::Vector2f position) {
    PolygonCollider::IncrementPosition(position);
}