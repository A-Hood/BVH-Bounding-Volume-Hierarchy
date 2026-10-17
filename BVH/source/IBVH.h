#pragma once
#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

struct NodePointer;
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
    virtual void Draw(sf::RenderTarget& _target, size_t _currentDepth) = 0;
    virtual const char* GetBVHName() = 0;

protected:
    // --- Collision (should not be here but for this demo its fine) ---
    virtual bool AABBCollision(const sf::FloatRect& _boxA, const sf::FloatRect& _boxB) const = 0;

    // --- Internal search ---
    virtual void RecursiveSearch(Collider& _targetObject, const NodePointer* _currentNode) = 0;
};