#include "Collider.h"

void Collider::CreateBoundingBox(sf::Vector2f _size)
{
    m_boundingBox.width = _size.x;
    m_boundingBox.height = _size.y;

    // Also defaults the centre of the bounding box to the middle
    m_centre.x = m_boundingBox.width / 2;
    m_centre.y = m_boundingBox.height / 2;
}

sf::FloatRect& Collider::GetBoundingBox() const
{
    return const_cast<Collider*>(this)->m_boundingBox;
}

float Collider::GetCentreFromAxis(const int axis) const
{
    return axis == 0 ? m_boundingBox.width : m_boundingBox.height;
}

void Collider::UpdateBoundingBoxPos(sf::Vector2f _position)
{
    m_boundingBox.left = _position.x;
    m_boundingBox.top = _position.y;
}
