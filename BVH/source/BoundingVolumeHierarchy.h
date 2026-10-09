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
    void CreateNewNode(Node* _currentNode, size_t _currentDepth);
    // 2.a Return true if X is the longest side, false is y is
    [[nodiscard]] inline bool IsXLongestSide(const Node* _currentNode) const;
    // 2.b Choose the split of the current node
    sf::Vector2i ChooseSplit(const Node* _currentNode);
    // 3. Grow the node bounding box based on the current collider
    void GrowBoundingBox(Node* _currentNode, Collider* _collider);



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
