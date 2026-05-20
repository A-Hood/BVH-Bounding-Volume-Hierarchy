#include "GameObject.h"

GameObject::GameObject(sf::FloatRect _rect)
{
    Initialise();
    SetPosition({_rect.left, _rect.top});
    SetSize({_rect.width, _rect.height});
}

void GameObject::Initialise()
{
    int rR = rand() % 255;
    int rG = rand() % 255;
    int rB = rand() % 255;
#if _BVHDEBUG
    rectVisual.setFillColor(sf::Color(rR, rG, rB));
#endif
}

void GameObject::SetPosition(const sf::Vector2f _position)
{
    m_position = _position;
    m_collider.UpdateBoundingBoxPos(m_position);
#if _BVHDEBUG
    // Debug only
    rectVisual.setPosition(_position.x, _position.y);
#endif
}

void GameObject::SetSize(const sf::Vector2f _size)
{
    m_collider.CreateBoundingBox(_size);
#if _BVHDEBUG
    // Debug only
    rectVisual.setSize({ _size.x, _size.y });
#endif
}

Collider& GameObject::GetCollider()
{
    return m_collider;
}

void GameObject::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
#if _BVHDEBUG
    target.draw(rectVisual, states);
#endif
}
