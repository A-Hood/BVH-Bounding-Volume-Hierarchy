#pragma once
#include "AbstractBVH.h"

class BVHIterationOne : AbstractBVH
{
public:
    // Constructor / Destructor
    BVHIterationOne() = default;
    ~BVHIterationOne() override;
    public:
    // --- Get reference to the colliders ---
    void AddCollider(Collider* _collider);
    // --- Create BVH ---
    void Generate() override;

    // --- DEBUG ---
    Node* GetMasterNode() const;
    void Draw(sf::RenderTarget& _target, Node* _currentNode, size_t _currentDepth) const;
private:
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

    // --- Dynamic BVH ---
    //void RecalculateBounds(Node* currentNode);
    private:
    // Hold the colliders used in the BVH
    std::vector<Collider*> m_colliders;
};
