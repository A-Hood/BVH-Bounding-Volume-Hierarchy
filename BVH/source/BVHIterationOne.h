#pragma once
#include "AbstractBVH.h"

class BVHIterationOne : public AbstractBVH
{
public:
    // Constructor / Destructor
    BVHIterationOne(const char* _bvhName) { m_bvhName = _bvhName; }
    ~BVHIterationOne() override;
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
    // 2.a Calculate the bounds of this new node
    Collider CalculateNodeBoundingBox(const std::vector<Collider*>& _nodeVector) const;
    // 2.b Return true if X is the longest side, false is y is
    [[nodiscard]] inline bool IsXLongestSide(const NodePointer* _currentNode) const;
    // 3. Objects are moved to either childA or childB
    void AssignObjectSide(std::vector<Collider*>& leftSide,
        std::vector<Collider*>& rightSide,
        NodePointer* _currentNode,
        float boundaryMidpoint) const;
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
