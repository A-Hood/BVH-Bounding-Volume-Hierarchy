#ifndef BVH_H
#define BVH_H

#include <vector>
#include <SFML/Graphics/RenderTarget.hpp>

#include "GameObject.h"
#include "Node.h"

struct SearchResult {
    // DEBUG
    float searchTime = 0;
    size_t numberCollidedObjects = 0;
};

class BVH
{
public:
    // Constructor / Destructor
    BVH() = default;
    BVH(size_t _maxDepth);
    ~BVH();
public:
    // --- Get reference to the colliders ---
    void CreateColliderRef(const std::vector<GameObject>& _colliderVecRef);
    // --- Create BVH ---
    void GenerateBVH();
    // --- Search ---
    SearchResult SearchBVH(GameObject& _targetRect);

    // --- DEBUG ---
    void DrawBVH(sf::RenderTarget& _target, size_t _currentDepth);
private:
    // --- Collision (should not be here but for this demo its fine) ---
    bool AABBCollision(const sf::FloatRect& _boxA, const sf::FloatRect& _boxB) const;

    // --- Generate BVH function steps ---
    // 1. Create a new node
    void CreateNewNode(Node& _currentNode, uint32_t parentIndex, size_t _currentDepth);
    // 2.a Finds the longest side of the bounding box
    [[nodiscard]] inline bool IsXLongestSide(const Node& _currentNode) const;
    // 2.b Create the split
    inline sf::Vector2i ChooseSplit(const Node& _currentNode) const;
    // 3. Calculate the bounds of this new node
    void GrowBoundingBox(Node& _currentNode, Collider& _collider) const;

    inline void DefineNodeType(Node* _currentNode, bool _nodeIsStatic) {}

    // --- Internal search ---
    void RecursiveSearch(Collider& _searchObject, const Node& _currentNode);

    // --- Dynamic BVH ---
    void RecalculateBounds(Node* currentNode) {}
private:
    // Hold the colliders used in the BVH
    std::vector<GameObject> m_gameObjects;

    // Keep track of all the nodes
    std::vector<Node> m_nodeVec;

    // Index of objects that collided with the subject collider
    std::vector<uint32_t> m_collidedObjectsQueue;

    // Parameters
    size_t m_maximumDepth = 50;
    size_t m_maxObjectsInLeafNode = 3;
};

#endif
