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

int Collider::GetCentreFromAxis(const int _splitAxis) const
{
    return _splitAxis == 0 ?
        static_cast<int>(m_boundingBox.left) + static_cast<int>(m_boundingBox.width / 2) :
        static_cast<int>(m_boundingBox.top) + static_cast<int>(m_boundingBox.height / 2);
}
