#include "Collider.h"

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
    m_vertices = new sf::Vertex[m_vertexCount];

    // DEBUG ONLY
    int rR = rand() % 255;
    int rG = rand() % 255;
    int rB = rand() % 255;
    //m_bbVisual.setFillColor(sf::Color(rR, rG, rB));
    //m_bbVisual.setSize({ m_size.x, m_size.y });

    m_vertices[0] = m_position + sf::Vector2f(0.f, 0.f);
    m_vertices[1] = m_position + sf::Vector2f(m_size.x, 0.0f);
    m_vertices[2] = m_position + sf::Vector2f(m_size.x, m_size.y);
    m_vertices[3] = m_position + sf::Vector2f(0.0f, m_size.y);

    // DEBUG Colour
    m_vertices[0].color = sf::Color(rR, rG, rB);
    m_vertices[1].color = sf::Color(rR, rG, rB);
    m_vertices[2].color = sf::Color(rR, rG, rB);
    m_vertices[3].color = sf::Color(rR, rG, rB);

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
    target.draw(m_vertices, 4, sf::PrimitiveType::Quads);
    //target.draw(m_bbVisual, states);
}


