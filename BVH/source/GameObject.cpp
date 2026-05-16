#include "GameObject.h"

void GameObject::Update()
{
    BoxCollider::Update();
}

void GameObject::SetObjectStatic()
{
    m_isStatic = true;
}

bool GameObject::IsObjectStatic()
{
    return m_isStatic;
}
