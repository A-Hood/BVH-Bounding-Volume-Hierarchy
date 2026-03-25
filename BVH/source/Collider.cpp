#include "Collider.h"

void BoxCollider::SetPosition(sf::Vector2f position)
{
    m_boundingBox.left = position.x;
    m_boundingBox.top = position.y;

    m_bbVisual.setPosition({m_boundingBox.left, m_boundingBox.top});
}

void BoxCollider::IncrementPosition(sf::Vector2f position)
{
    m_boundingBox.left += position.x;
    m_boundingBox.top += position.y;

    m_bbVisual.setPosition({m_boundingBox.left, m_boundingBox.top});
}

sf::Vector2f BoxCollider::GetPosition()
{
    return {m_boundingBox.left, m_boundingBox.top};
}

void BoxCollider::SetOrigin(sf::Vector2f origin)
{
    m_origin = origin;
    m_bbVisual.setOrigin(m_origin);
}

sf::Vector2f BoxCollider::GetOrigin()
{
    return m_origin;
}

void BoxCollider::SetSize(sf::Vector2f size)
{
    m_boundingBox.width = size.x;
    m_boundingBox.height = size.y;
}

sf::Vector2f BoxCollider::GetSize()
{
    return {m_boundingBox.width, m_boundingBox.height};
}

void BoxCollider::CreateCollider()
{
    // DEBUG ONLY
    int rR = rand() % 255;
    int rG = rand() % 255;
    int rB = rand() % 255;
    m_bbVisual.setFillColor(sf::Color(rR, rG, rB));
    m_bbVisual.setSize({ m_boundingBox.width, m_boundingBox.height });
    /*
    m_vertices[0] = sf::Vector2f(0.f, 0.f);
    m_vertices[1] = sf::Vector2f(m_boundingBox.left, 0.0f);
    m_vertices[2] = sf::Vector2f(m_boundingBox.left, m_boundingBox.top);
    m_vertices[3] = sf::Vector2f(0.0f, m_boundingBox.top);
    */
}

sf::FloatRect BoxCollider::GetBoundingBox() const
{
    return m_boundingBox;
}

void BoxCollider::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_bbVisual, states);
}


