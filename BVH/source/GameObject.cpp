#include "GameObject.h"

GameObject::GameObject(sf::Vector2f position, sf::Vector2f size, size_t vertexCount)
{
    BoxCollider::SetPosition(position);
    BoxCollider::SetSize(size);
    BoxCollider::SetOrigin({size.x / 2.f, size.y  /2.f});
    //BoxCollider::SetOrigin({0, 0});
    BoxCollider::SetVertexCount(vertexCount);
    BoxCollider::CreateCollider();
}

void GameObject::Update()
{
    BoxCollider::UpdateCollider();
}

void GameObject::SetObjectStatic()
{
    m_isStatic = true;
}

bool GameObject::IsObjectStatic()
{
    return m_isStatic;
}
