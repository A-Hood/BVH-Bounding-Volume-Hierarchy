#pragma once
#include "Collider.h"

class GameObject : public BoxCollider
{
public:
    GameObject(sf::Vector2f position, sf::Vector2f size, size_t vertexCount) :
        BoxCollider(position, size)
    {
        BoxCollider::SetPosition(position);
        BoxCollider::SetSize(size);
        BoxCollider::SetOrigin({ size.x / 2.f, size.y / 2.f });
        //BoxCollider::SetOrigin({0, 0});
        BoxCollider::SetVertexCount(vertexCount);
        BoxCollider::CreateCollider();
    }
    GameObject() : BoxCollider({ 0.0f, 0.0f }, { 0.0f, 0.0f }) {}
    ~GameObject() override = default;

public:
    void Update();

    void SetObjectStatic();
    bool IsObjectStatic();

private:
    bool m_isStatic = false;
};
