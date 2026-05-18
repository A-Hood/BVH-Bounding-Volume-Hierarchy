#include "Collider.h"

sf::FloatRect& Collider::GetBoundingBox()
{
    //return const_cast<Collider*>(this)->m_boundingBox;
    return m_boundingBox;
}
int Collider::GetCentreFromAxis(const int axis) const
{
    return axis == 0 ? static_cast<int>(m_boundingBox.width) : static_cast<int>(m_boundingBox.height);
}
void Collider::CreateBoundingBox(sf::Vector2f _size)
{
    m_boundingBox.width = _size.x;
    m_boundingBox.height = _size.y;

    // Also defaults the centre of the bounding box to the middle
    m_centre.x = m_boundingBox.width / 2;
    m_centre.y = m_boundingBox.height / 2;
}
void Collider::UpdateBoundingBoxPos(sf::Vector2f _position)
{
    m_boundingBox.left = _position.x;
    m_boundingBox.top = _position.y;
}

