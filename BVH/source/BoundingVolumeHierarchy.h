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
    ~BVH();
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
    // 1. Create a new node
    void CreateNewNode(Node* currentNode, size_t currentDepth);
    // 2. Calculate the bounds of this new node
    FloatRect CalculateNodeBoundingBox(const std::vector<Collider*>& nodeVector) const;
    // 3. Split the node BB into x pieces to find the best area
    float CalculateBestMidpoint(Node* _currentNode);
    // Finds the longest side of the bounding box
    [[nodiscard]] inline bool IsXLongestSide(const FloatRect& boundingBox) const;
    float CalculateAreaOfBoundingBox(const FloatRect& _box) const;
	// Objects are moved to either childA or childB
    void AssignObjectSide(std::vector<Collider*>& leftSide,
	    std::vector<Collider*>& rightSide,
	    Node* currentNode,
	    float boundaryMidpoint);

    inline void DefineNodeType(Node* _currentNode, bool _nodeIsStatic);

    // --- Destroy BVH ---
    void TraversalNodeDestroy(const Node* _currentNode);

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
    size_t m_maximumDepth = 30;
    size_t m_maxObjectsInLeafNode = 3;
    // The bounding box of the node will be split into x pieces to find the best size of that box.
    size_t m_maxSliceTests = 5;
};

#endif
