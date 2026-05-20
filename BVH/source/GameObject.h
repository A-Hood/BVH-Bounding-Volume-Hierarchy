#pragma once
#include <SFML/Graphics/RectangleShape.hpp>

#include "Collider.h"

class GameObject : public sf::Drawable
{
public:
    GameObject() = default;
    GameObject(sf::FloatRect _rect);
    ~GameObject() override = default;

public:
    void Initialise();
    void SetPosition(sf::Vector2f _position);
    void SetSize(sf::Vector2f _size);

    Collider& GetCollider();

    // DEBUG DRAW
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    // DEBUG ONLY
#if _BVHDEBUG
    sf::RectangleShape rectVisual;
#endif
    // Position
    sf::Vector2f m_position;

    Collider m_collider;
};
