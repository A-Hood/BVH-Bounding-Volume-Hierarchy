#pragma once
#include <SFML/Graphics/RectangleShape.hpp>

#include "Collider.h"

class GameObject : public sf::Drawable
{
public:
    GameObject() = default;
    GameObject(sf::FloatRect _rect);
    ~GameObject() = default;

public:
    void Initialise();
    // Transforms
    void SetPosition(sf::Vector2f _position);
    void IncrementPosition(sf::Vector2f _position);
    // Size
    void SetSize(sf::Vector2f _size);

    // Color
    void SetColor(sf::Color _color);

    // Get functions
    sf::Vector2f GetPosition() const;
    sf::Vector2f GetSize();

    Collider& GetCollider();

    // Debug only
    sf::VertexArray& GetVertexArray();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    // DEBUG ONLY
    sf::VertexArray m_vertices;
    // Position
    sf::Vector2f m_position;

    Collider m_collider;
};
