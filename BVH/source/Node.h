#pragma once
#include <SFML/Graphics/Rect.hpp>

struct Node
{
    sf::FloatRect boundingBox = {0, 0, 0, 0};
    uint32_t objectIndex = 0;
    uint32_t objectCount = 0;
    uint32_t childIndex = 0;

    uint32_t parentIndex = 0;

    // DEBUGGING ONLY
#if _BVHDEBUG
    uint32_t currentDepth = 0;
#endif
};