#ifndef BVH_H
#define BVH_H

#include <vector>
#include "Node.h"

#include "GameObject.h"

struct SearchResult {
    float searchTime = 0;
    // Collided objects with the subject
    std::vector<Collider*> m_collidedObjectsQueue;
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
    void AddCollider(Collider* _collider);
    // --- Create BVH ---
    void Generate();
    // --- Search ---
    SearchResult SearchForCollision(GameObject& _targetObject);

    // --- DEBUG ---
    Node* GetMasterNode() const;
    void Draw(sf::RenderTarget& _target, Node* _currentNode, size_t _currentDepth) const;
private:
    // --- Collision (should not be here but for this demo its fine) ---
    bool AABBCollision(const sf::FloatRect& _boxA, const sf::FloatRect& _boxB) const;

    // --- Generate BVH function steps ---
    // 1. Create a new node
    void CreateNewNode(Node* currentNode, size_t currentDepth);
    // 2. Calculate the bounds of this new node
    Collider CalculateNodeBoundingBox(const std::vector<Collider*>& nodeVector) const;
    // Finds the longest side of the bounding box
    [[nodiscard]] inline bool IsXLongestSide(const sf::FloatRect& boundingBox) const;
	// Objects are moved to either childA or childB
    void AssignObjectSide(std::vector<Collider*>& leftSide,
	    std::vector<Collider*>& rightSide,
	    Node* currentNode,
	    float boundaryMidpoint);

    // --- Destroy BVH ---
    void TraversalNodeDestroy(const Node* _currentNode);

    // --- Internal search ---
    void RecursiveSearch(Collider& _targetObject, const Node* _currentNode);

    // --- Dynamic BVH ---
    //void RecalculateBounds(Node* currentNode);
private:
    SearchResult m_recursiveSearch;
    // Hold the colliders used in the BVH
    std::vector<Collider*> m_colliders;
    // Keep track of the master node
    Node* m_masterNode = nullptr;

    // Parameters
    size_t m_maximumDepth = 50;
    size_t m_maxObjectsInLeafNode = 3;
};

#endif
