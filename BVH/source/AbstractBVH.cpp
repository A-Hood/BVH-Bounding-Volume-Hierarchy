#include "AbstractBVH.h"

#include <chrono>

#include "Collider.h"
#include "GameObject.h"
#include "Node.h"

AbstractBVH::AbstractBVH(size_t _maxDepth, size_t _maxObjectsInLeafNode)
{
    m_maximumDepth = _maxDepth;
    m_maxObjectsInLeafNode = _maxObjectsInLeafNode;
}

SearchResult AbstractBVH::SearchForCollision(GameObject& _targetObject)
{
    // Traverse through the bvh, then check objects within that node
    auto t1 = std::chrono::high_resolution_clock::now();
    RecursiveSearch(_targetObject.GetCollider(), m_masterNode); // start search at master node
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> bvhSearch = t2 - t1;

    m_recursiveSearch.searchTime = bvhSearch.count();
    return m_recursiveSearch;
}
bool AbstractBVH::AABBCollision(const sf::FloatRect& _boxA, const sf::FloatRect& _boxB) const
{
    return _boxA.left <= _boxB.left + _boxB.width &&
        _boxA.top <= _boxB.top + _boxB.height &&
        _boxA.left + _boxA.width >= _boxB.left &&
        _boxA.top + _boxA.height >= _boxB.top;
}

void AbstractBVH::RecursiveSearch(Collider& _targetObject, const Node* _currentNode)
{
    // If the searchRect is not within this current node, do not proceed
    if (!AABBCollision(_targetObject.GetBoundingBox(), _currentNode->m_boundingBox))
    {
        return;
    }
    // If the child nodes are a nullptr, therefore this is the leaf node.
    // Check objects within the leaf node
    // Else if this is not a leaf node, recurse
    if (_currentNode->m_childA != nullptr)
    {
        RecursiveSearch(_targetObject, _currentNode->m_childA);
    }
    if (_currentNode->m_childB != nullptr)
    {
        RecursiveSearch(_targetObject, _currentNode->m_childB);
        return;
    }
    // If this is not a nullptr, the searchRect is within this node, then write it down
    // Check collisions with object inside of node
    for (const auto collider : _currentNode->m_nodeColliders)
    {
        if (AABBCollision(_targetObject.GetBoundingBox(), collider->GetBoundingBox()))
        {
            m_recursiveSearch.m_collidedObjectsQueue.emplace_back(collider);
        }
    }
}
