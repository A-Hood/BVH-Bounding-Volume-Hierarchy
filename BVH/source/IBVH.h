#pragma once
#include <vector>
#include <SFML/Graphics/RenderTarget.hpp>

struct Node;
class GameObject;
class Collider;

struct SearchResult {
    float searchTime = 0;
    // Collided objects with the subject
    std::vector<Collider*> m_collidedObjectsQueue;
};

class IBVH
{
public:
    virtual ~IBVH() = default;

public:
    virtual void Generate() = 0;
    virtual SearchResult SearchForCollision(GameObject& _targetObject) = 0;

protected:
    // --- Collision (should not be here but for this demo its fine) ---
    virtual bool AABBCollision(const sf::FloatRect& _boxA, const sf::FloatRect& _boxB) const = 0;

    // --- Internal search ---
    virtual void RecursiveSearch(Collider& _targetObject, const Node* _currentNode) = 0;
};