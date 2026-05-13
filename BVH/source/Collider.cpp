#include "Collider.h"

#include <valarray>

#include "Application.h"

// Base collider --------------------------------------------------------------------------------------------------------------
void Collider::SetPosition(sf::Vector2f position)
{
    m_position = position;
}

void Collider::IncrementPosition(sf::Vector2f position)
{
    m_position += position;
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

bool Collider::CircleCircleCollision(const Collider* colliderA, const Collider* colliderB)
{
    return false;
}

bool Collider::BoxCircleCollision(const BoxCollider* colliderA, const CircleCollider* colliderB)
{
    return false;
}

bool Collider::BoxBoxCollision(const BoxCollider* colliderA, const BoxCollider* colliderB)
{
    return false;
}

// Circle Collider --------------------------------------------------------------------------------------------------------------
void CircleCollider::CreateCollider()
{

}

bool CircleCollider::CollideWith(Collider* otherCollider) const
{
    return false;
}

bool CircleCollider::CollideWith(BoxCollider* otherCollider) const
{
    return Collider::BoxCircleCollision(otherCollider, this);
}

bool CircleCollider::CollideWith(CircleCollider* otherCollider) const
{
    return Collider::CircleCircleCollision(otherCollider, this);
}

// Box Collider --------------------------------------------------------------------------------------------------------------

void BoxCollider::CreateCollider()
{
    //m_vertices = new sf::Vertex[m_vertexCount];
    //// DEBUG ONLY
    //int rR = rand() % 255;
    //int rG = rand() % 255;
    //int rB = rand() % 255;
    //
    //sf::Vector2f pos = m_position - m_origin;
    //m_vertices[0] = pos + sf::Vector2f(0.f, 0.f);
    //m_vertices[1] = pos + sf::Vector2f(m_size.x, 0.0f);
    //m_vertices[2] = pos + sf::Vector2f(m_size.x, m_size.y);
    //m_vertices[3] = pos + sf::Vector2f(0.0f, m_size.y);
    //
    //// DEBUG Colour
    //m_vertices[0].color = sf::Color(rR, rG, rB);
    //m_vertices[1].color = sf::Color(rR, rG, rB);
    //m_vertices[2].color = sf::Color(rR, rG, rB);
    //m_vertices[3].color = sf::Color(rR, rG, rB);

}

bool BoxCollider::CollideWith(Collider* otherCollider) const
{
    return false;
}

bool BoxCollider::CollideWith(BoxCollider* otherCollider) const
{
    return Collider::BoxBoxCollision(otherCollider, this);
}

bool BoxCollider::CollideWith(CircleCollider* otherCollider) const
{
    return Collider::BoxCircleCollision(this, otherCollider);
}

void BoxCollider::SetSize(sf::Vector2f size)
{
    m_size = size;
}

sf::Vector2f BoxCollider::GetSize()
{
    return m_size;
}

void BoxCollider::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    //target.draw(m_vertices, 4, sf::PrimitiveType::Quads);
    //target.draw(m_bbVisual, states);
}

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

bool PolygonCollider::CollideWith(BoxCollider* otherCollider) const {
    return false;
}

bool PolygonCollider::CollideWith(CircleCollider* otherCollider) const {
    return false;
}

void PolygonCollider::IncrementPosition(sf::Vector2f _pos) {
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


