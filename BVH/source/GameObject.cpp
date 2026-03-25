#include "GameObject.h"

GameObject::GameObject(sf::Vector2f position, sf::Vector2f size, size_t vertexCount)
{
    BoxCollider::SetPosition(position);
    BoxCollider::SetSize(size);
    BoxCollider::SetVertexCount(vertexCount);
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
