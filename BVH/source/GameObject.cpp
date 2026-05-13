#include "GameObject.h"

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
