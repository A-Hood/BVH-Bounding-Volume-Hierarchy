#ifndef BVH_H
#define BVH_H

#include <vector>

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
    ~BVH() = default;
public:
    // --- Add colliders to the bvh ---
    void AddCollider(Collider* _collider);
    // --- Create BVH ---
    void Generate();
    // --- Search ---
    SearchResult SearchBVH(const FloatRect& _targetRect);

    // --- DEBUG ---
    Node* GetMasterNode() const;
    void DrawBVH(sf::RenderTarget& _target, Node* _currentNode, size_t _currentDepth) const;
private:
    // --- Collision (should not be here but for this demo its fine) ---
    bool AABBCollision(const FloatRect& _boxA, const FloatRect& _boxB) const;

    // --- Generate BVH function steps ---
    void CreateNewNode(Node* currentNode, size_t currentDepth, size_t maximumDepth);
    FloatRect CalculateNodeBoundingBox(const std::vector<Collider*>& nodeVector) const;
    [[nodiscard]] inline bool IsXLongestSide(const FloatRect& boundingBox) const;
	void AssignObjectSide(std::vector<Collider*>& leftSide,
	    std::vector<Collider*>& rightSide,
	    Node* currentNode,
	    float boundaryMidpoint);
    inline void DefineNodeType(Node* _currentNode, bool _nodeIsStatic);

    // --- Internal search ---
    void RecursiveSearch(const FloatRect& _searchRect, const Node* _currentNode);

    // --- Dynamic BVH ---
    void RecalculateBounds(Node* currentNode);
private:
    std::vector<Collider*> m_colliders;
    // Keep track of the master node
    Node* m_masterNode = nullptr;
    // Collided objects with the subject
    std::vector<Collider*> m_collidedObjectsQueue;

    // Dynamic nodes that need to be updated if the objects inside the leaf nodes move
	std::vector<Node*> m_dynamicNodeQueue;

    // Parameters
    size_t m_maximumDepth;
    size_t m_maxObjectsInLeafNode = 3;
};

#endif
