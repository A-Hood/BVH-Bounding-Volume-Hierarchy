#include "GameObject.h"

void GameObject::SetPosition(sf::Vector2f _newPosition)
{
    m_colliderBox.left = _newPosition.x;
    m_colliderBox.top = _newPosition.y;

    UpdateDebugVisualBox();
}

void GameObject::IncrementPosition(sf::Vector2f _increment)
{
    m_colliderBox.left += _increment.x;
    m_colliderBox.top += _increment.y;

    UpdateDebugVisualBox();
}

FloatRect& GameObject::GetBoundingBox()
{
    return m_colliderBox;
}

void GameObject::CreateDebugVisualBox()
{
    m_rectVisual.setPosition(m_colliderBox.left, m_colliderBox.top);
    m_rectVisual.setSize({ m_colliderBox.width, m_colliderBox.height });

    int rR = rand() % 255;
    int rG = rand() % 255;
    int rB = rand() % 255;
    m_rectVisual.setFillColor(sf::Color(rR, rG, rB));
}

void GameObject::UpdateDebugVisualBox()
{
    m_rectVisual.setPosition(m_colliderBox.left, m_colliderBox.top);
}
