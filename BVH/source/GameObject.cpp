#include "GameObject.h"

void GameObject::Initialise()
{
    int rR = rand() % 255;
    int rG = rand() % 255;
    int rB = rand() % 255;
    rectVisual.setFillColor(sf::Color(rR, rG, rB));
}

void GameObject::SetPosition(const sf::Vector2f _position)
{
    m_position = _position;
    m_collider.UpdateBoundingBoxPos(m_position);
    // Debug only
    rectVisual.setPosition(_position.x, _position.y);
}

void GameObject::SetSize(const sf::Vector2f _size)
{
    m_collider.CreateBoundingBox(_size);
    // Debug only
    rectVisual.setSize({ _size.x, _size.y });
}

Collider& GameObject::GetCollider()
{
    return m_collider;
}

void GameObject::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(rectVisual, states);
}
