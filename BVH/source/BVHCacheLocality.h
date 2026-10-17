#pragma once
#include "AbstractBVH.h"
#include "NodeInt.h"

class BVHCacheLocality : public AbstractBVH
{
public:
    BVHCacheLocality(const char* _bvhName) { m_bvhName = _bvhName; }
    ~BVHCacheLocality() override = default;
public:
    // --- Get reference to the colliders ---
    void CreateColliderRef(const std::vector<GameObject>& _colliderVecRef);
    // --- Create BVH ---
    void Generate() override;

    void Draw(sf::RenderTarget& _target, size_t _currentDepth) override;
private:
    // --- Generate BVH function steps ---
    // 1. Create a new node
    void CreateNewNode(NodeInt& _currentNode, uint32_t parentIndex, size_t _currentDepth);
    // 2.a Finds the longest side of the bounding box
    [[nodiscard]] inline bool IsXLongestSide(const NodeInt& _currentNode) const;
    // 2.b Create the split
    inline sf::Vector2i ChooseSplit(const NodeInt& _currentNode) const;
    // 3. Calculate the bounds of this new node
    void GrowBoundingBox(NodeInt& _currentNode, Collider& _collider) const;

    // --- Dynamic BVH ---
    //void RecalculateBounds(NodeInt* currentNode) {}
private:
    // Hold the colliders used in the BVH
    std::vector<GameObject> m_gameObjects;

    // Keep track of all the nodes
    std::vector<NodeInt> m_nodeVec;

    // Index of objects that collided with the subject collider
    std::vector<uint32_t> m_collidedObjectsQueue;
};
