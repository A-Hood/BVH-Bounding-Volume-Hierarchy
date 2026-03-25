#include "GameObject.h"

GameObject::GameObject(sf::Vector2f position, sf::Vector2f size)
{
    BoxCollider::SetPosition(position);
    BoxCollider::SetSize(size);
    BoxCollider::CreateCollider();
}

void GameObject::SetObjectStatic()
{
    m_isStatic = true;
}

bool GameObject::IsObjectStatic()
{
    return m_isStatic;
}
