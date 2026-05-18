#pragma once
#include <SFML/Graphics/RectangleShape.hpp>

#include "Collider.h"

class GameObject : public Collider, public sf::Drawable
{
public:
    GameObject() = default;
    ~GameObject() = default;

public:
    void Initialise();
    void SetPosition(sf::Vector2f _position);
    void SetSize(sf::Vector2f _size);

    // DEBUG DRAW
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    // DEBUG ONLY
    sf::RectangleShape rectVisual;
    // Position
    sf::Vector2f m_position;
};
