#pragma once
#include "Collider.h"

class GameObject : public BoxCollider
{
public:
    GameObject() = default;
    GameObject(sf::Vector2f position, sf::Vector2f size, size_t vertexCount);
    ~GameObject() override = default;

public:
    void Update();

    void SetObjectStatic();
    bool IsObjectStatic();

private:
    bool m_isStatic = false;
};
