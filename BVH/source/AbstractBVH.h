#pragma once
#include "IBVH.h"

class AbstractBVH : public IBVH
{
public:
    AbstractBVH() = default;
    AbstractBVH(size_t _maxDepth, size_t _maxObjectsInLeafNode);

public:
    SearchResult SearchForCollision(GameObject& _targetObject) override;
    const char* GetBVHName() override;

protected:
    bool AABBCollision(const sf::FloatRect& _boxA, const sf::FloatRect& _boxB) const override;

    void RecursiveSearch(Collider& _targetObject, const Node* _currentNode) override;
protected:
    SearchResult m_recursiveSearch;
    // Parameters
    size_t m_maximumDepth = 50;
    size_t m_maxObjectsInLeafNode = 3;

    const char* m_bvhName;

    // Keep track of the master node
    Node* m_masterNode = nullptr;
};
