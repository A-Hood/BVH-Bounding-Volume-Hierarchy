#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <vector>
class Collider;

struct Node
{
    // Holds pointers to gameObjects
    std::vector<Collider*> m_nodeColliders;
    // Bounds of the node
    sf::FloatRect m_boundingBox = {0, 0, 0, 0};
    // Not currently used for anything
    Node* m_parentNode = nullptr;

    Node* m_childA = nullptr;
    Node* m_childB = nullptr;

    #if _BVHDEBUG
    uint32_t m_currentDepth = 0;
    #endif

};
