#include "Collider.h"

void Collider::CreateBoundingBox(sf::Vector2f _position, sf::Vector2f _size)
{
    m_boundingBox.left = _position.x;
    m_boundingBox.top = _position.y;

    m_boundingBox.width = _size.x;
    m_boundingBox.height = _size.y;
}

void Collider::SetBoundingBoxSize(sf::Vector2f _size)
{
    m_boundingBox.width = _size.x;
    m_boundingBox.height = _size.y;
}

void Collider::SetBoundingBoxPosition(sf::Vector2f _position)
{
    m_boundingBox.left = _position.x;
    m_boundingBox.top = _position.y;
}
const sf::FloatRect& Collider::GetBoundingBox() const
{
    return m_boundingBox;
}
