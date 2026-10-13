#pragma once
#include "AbstractBVH.h"

class BVHIterationTwo : public AbstractBVH
{
public:
    // Constructor / Destructor
    BVHIterationTwo(const char* _bvhName) { m_bvhName = _bvhName; }
    ~BVHIterationTwo() override;
public:
    // --- Get reference to the colliders ---
    void AddCollider(Collider* _collider);
    // --- Create BVH ---
    void Generate() override;

    // --- DEBUG ---
    void Draw(sf::RenderTarget& _target, size_t _currentDepth) override;
private:
    // --- Generate BVH function steps ---
    // 1. Create a new node
    void CreateNewNode(NodePointer* _currentNode, size_t _currentDepth);
    // 2.a Return true if X is the longest side, false is y is
    [[nodiscard]] inline bool IsXLongestSide(const NodePointer* _currentNode) const;
    // 2.b Choose the split of the current node
    sf::Vector2i ChooseSplit(const NodePointer* _currentNode);
    // 3. Grow the node bounding box based on the current collider
    void GrowBoundingBox(NodePointer* _currentNode, Collider* _collider);
    // DEBUG ONLY
    // 4. Draw BVH
    void RecursiveDraw(sf::RenderTarget& _target, NodePointer* _currentNode, size_t _currentDepth);

    // --- Destroy BVH ---
    void TraversalNodeDestroy(const NodePointer* _currentNode);

    // --- Dynamic BVH ---
    //void RecalculateBounds(NodePointer* currentNode);
private:
    // Hold the colliders used in the BVH
    std::vector<Collider*> m_colliders;
};
