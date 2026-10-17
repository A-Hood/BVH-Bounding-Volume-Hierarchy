#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <vector>

struct NodePointer
{
    // Holds pointers to gameObjects
    std::vector<Collider*> m_nodeColliders;
    // Bounds of the node
    sf::FloatRect m_boundingBox = {0, 0, 0, 0};
    // Not currently used for anything
    NodePointer* m_parentNode = nullptr;

    NodePointer* m_childA = nullptr;
    NodePointer* m_childB = nullptr;

    #if _BVHDEBUG
    uint32_t m_currentDepth = 0;
    #endif

};
