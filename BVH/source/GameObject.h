#pragma once
#include <cstdint>
#include <SFML/Graphics/RectangleShape.hpp>

#include "FloatRect.h"

class GameObject
{
public:
    GameObject() = default;
    ~GameObject() = default;

public:
    // --- Transforms ---
    // ### Position ###
    void SetPosition(sf::Vector2f _newPosition);
    void IncrementPosition(sf::Vector2f _increment);
    // ### Area ###
    FloatRect& GetBoundingBox();

private:
    void CreateDebugVisualBox();
    void UpdateDebugVisualBox();


private:

    // DEBUG ONLY
    sf::RectangleShape m_rectVisual;

    uint16_t m_id;
    FloatRect m_colliderBox;
};
