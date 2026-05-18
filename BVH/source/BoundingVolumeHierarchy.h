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
    void CreateColliderRef(std::vector<GameObject>& _colliderVecRef);
    // --- Create BVH ---
    void GenerateBVH();
    // --- Search ---
    SearchResult SearchBVH(const sf::FloatRect& _targetRect) { return {}; }

    // --- DEBUG ---
    void DrawBVH(sf::RenderTarget& _target, size_t _currentDepth);
private:
    // --- Collision (should not be here but for this demo its fine) ---
    bool AABBCollision(const sf::FloatRect& _boxA, const sf::FloatRect& _boxB) {}

    // --- Generate BVH function steps ---
    void InitialiseNodeBoundingBox(Node& _currentNode);
    // 1. Create a new node
    void CreateNewNode(Node& _currentNode, size_t _currentDepth);
    // 2. Create the split
    inline sf::Vector2i ChooseSplit(const Node& _currentNode) const;
    // 2. Calculate the bounds of this new node
    void GrowBoundingBox(Node& _currentNode, Collider& _collider);
    // Finds the longest side of the bounding box
    [[nodiscard]] inline bool IsXLongestSide(const Node& _currentNode) const;


    inline void DefineNodeType(Node* _currentNode, bool _nodeIsStatic) {}

    // --- Destroy BVH ---
    void TraversalNodeDestroy(const Node* _currentNode) {}

    // --- Internal search ---
    void RecursiveSearch(const sf::FloatRect& _searchRect, const Node* _currentNode) {}

    // --- Dynamic BVH ---
    void RecalculateBounds(Node* currentNode) {}
private:
    // Reference of the colliders
    std::vector<Collider*> m_colliders;

    // Keep track of all the nodes
    std::vector<Node> m_nodeVec;

    // Collided objects with the subject
    std::vector<uint32_t> m_collidedObjectsQueue;

    // Dynamic nodes that need to be updated if the objects inside the leaf nodes move
	std::vector<Node*> m_dynamicNodeQueue;

    // Parameters
    size_t m_maximumDepth = 30;
    size_t m_maxObjectsInLeafNode = 3;
};

#endif
