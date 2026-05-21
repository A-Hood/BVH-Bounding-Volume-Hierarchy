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
    void IncrementPosition(sf::Vector2f _position);
    void SetSize(sf::Vector2f _size);

    Collider& GetCollider();

#ifndef _BVHDEBUG
    // Draw only done in release mode to stop abstraction error
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {}
#endif

    // DEBUG DRAW
#ifdef _BVHDEBUG
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    sf::RectangleShape& GetDebugShape();
#endif

private:
    // DEBUG ONLY
#if _BVHDEBUG
    sf::RectangleShape m_rectVisual;
#endif
    // Position
    sf::Vector2f m_position;

    Collider m_collider;
};
