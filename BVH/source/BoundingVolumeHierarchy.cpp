#include "BoundingVolumeHierarchy.h"

#include <algorithm>
#include <chrono>
#include <iostream>

#include "GameObject.h"
#include "Application.h"

BVH::BVH(size_t _maxDepth)
{
    m_maximumDepth = _maxDepth;
}

BVH::~BVH()
{
    m_gameObjects.clear();
    m_nodeVec.clear();
}

void BVH::CreateColliderRef(const std::vector<GameObject>& _colliderVecRef)
{
    m_gameObjects = _colliderVecRef;
    m_nodeVec.reserve((2 * m_gameObjects.size()) - 1);
}

void BVH::GenerateBVH()
{
    auto t1 = std::chrono::system_clock::now();
    // Create master node - Master node is the first node in the vector
    Node& masterNode = m_nodeVec.emplace_back();
    masterNode.objectIndex = 0;
    masterNode.objectCount = m_gameObjects.size();

    // Find the size of the master node
    for (auto index = masterNode.objectIndex; index < masterNode.objectIndex + masterNode.objectCount; index++)
    {
        GrowBoundingBox(masterNode, m_gameObjects[index].GetCollider());
    }

    // Move into the vector
    CreateNewNode(masterNode, 0, 1);

    auto t2 = std::chrono::system_clock::now();
    std::chrono::duration<float, std::milli> time = t2 - t1;
    LOG("Time to create BVH: " + std::to_string(time.count()) + "ms");

}

SearchResult BVH::SearchBVH(GameObject& _targetRect)
{
    m_collidedObjectsQueue.clear();

    // Traverse through the bvh, then check objects within that node
    auto t1 = std::chrono::high_resolution_clock::now();
    RecursiveSearch(_targetRect.GetCollider(), m_nodeVec[0]); // Start search at master node
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> bvhSearch = t2 - t1;


    // Set result of search
    SearchResult result;
    result.searchTime = bvhSearch.count();
    result.numberCollidedObjects = m_collidedObjectsQueue.size();

    return result;
}

void BVH::DrawBVH(sf::RenderTarget& _target, size_t _currentDepth)
{
#if _BVHDEBUG
    for (Node& currentNode : m_nodeVec)
    {
        if (currentNode.currentDepth != _currentDepth)
        {
            continue;
        }
        sf::RectangleShape debugRect;
        debugRect.setPosition(currentNode.boundingBox.left, currentNode.boundingBox.top);
        debugRect.setSize({currentNode.boundingBox.width, currentNode.boundingBox.height});
        debugRect.setOutlineColor(sf::Color::Red);
        debugRect.setOutlineThickness(3);
        debugRect.setFillColor(sf::Color(0, 0, 0, 0));

        _target.draw(debugRect);
    }
#endif
}

bool BVH::AABBCollision(const sf::FloatRect& _boxA, const sf::FloatRect& _boxB) const
{
    return _boxA.left <= _boxB.left + _boxB.width &&
        _boxA.top <= _boxB.top + _boxB.height &&
        _boxA.left + _boxA.width >= _boxB.left &&
        _boxA.top + _boxA.height >= _boxB.top;
}

void BVH::CreateNewNode(Node& _currentNode, const uint32_t parentIndex, const size_t _currentDepth)
{
    if (_currentDepth >= m_maximumDepth || _currentNode.objectCount <= m_maxObjectsInLeafNode)
    {
        // Do not continue as we have hit the max size
        return;
    }
    // Split the current node bounding box
    sf::Vector2i result = ChooseSplit(_currentNode);
    const int splitAxis = result.x;
    const int splitPosition = result.y;

    // Store where the starting position of the children are
    // This will technically be the index just after the parent index
    _currentNode.childIndex = m_nodeVec.size();

    uint32_t currentNodeVecSize = _currentNode.childIndex;

    // Create child nodes
    Node& childA = m_nodeVec.emplace_back();
    childA.objectIndex = _currentNode.objectIndex;
    childA.parentIndex = parentIndex;

    Node& childB = m_nodeVec.emplace_back();
    childB.objectIndex = _currentNode.objectIndex;
    childB.parentIndex = parentIndex;

#if _BVHDEBUG
    // DEBUGGING ONLY
    childA.currentDepth = _currentDepth;
    childB.currentDepth = _currentDepth;
#endif

    for (auto index = _currentNode.objectIndex; index < _currentNode.objectIndex + _currentNode.objectCount; index++)
    {
        const bool isSideA = m_gameObjects[index].GetCollider().GetCentreFromAxis(splitAxis) < splitPosition;
        Node& currentChild = isSideA ? childA : childB;
        // Changes the size of the bounding box of the child node using the current collider
        GrowBoundingBox(currentChild, m_gameObjects[index].GetCollider());
        currentChild.objectCount++;

        if (isSideA)
        {
            const uint32_t swap = currentChild.objectIndex + currentChild.objectCount - 1;
            std::swap(m_gameObjects[index], m_gameObjects[swap]);
            childB.objectIndex++;
        }
    }

    CreateNewNode(childA, currentNodeVecSize, _currentDepth + 1);
    CreateNewNode(childB, currentNodeVecSize + 1, _currentDepth + 1);
}
bool BVH::IsXLongestSide(const Node& _currentNode) const
{
    return _currentNode.boundingBox.width > _currentNode.boundingBox.height;
}
sf::Vector2i BVH::ChooseSplit(const Node& _currentNode) const
{
    // X returns the splitAxis
    // Y returns the splitPosition
    if (IsXLongestSide(_currentNode))
    {
        // X is the longest
        return {0, static_cast<int>(_currentNode.boundingBox.left) + (static_cast<int>(_currentNode.boundingBox.width) / 2)};
    }
    return {1, static_cast<int>(_currentNode.boundingBox.top) + (static_cast<int>(_currentNode.boundingBox.height) / 2)};
}

void BVH::GrowBoundingBox(Node& _currentNode, Collider& _collider)
{
    auto& boundingBox = _collider.GetBoundingBox();
    if (_currentNode.boundingBox.width <= 0 || _currentNode.boundingBox.height <= 0)
    {
        // We know this node is new.
        // Therefore, we must set the position and size to the current collider
        // Return after
        _currentNode.boundingBox.left = boundingBox.left;
        _currentNode.boundingBox.top = boundingBox.top;
        _currentNode.boundingBox.width = boundingBox.width;
        _currentNode.boundingBox.height = boundingBox.height;
        return;
    }
    // Left
    if (_currentNode.boundingBox.left > boundingBox.left)
    {
        // Update the width
        if (_currentNode.boundingBox.width > 0)
        {
            _currentNode.boundingBox.width += (_currentNode.boundingBox.left - boundingBox.left);
        }
        // Update the node BB left
        _currentNode.boundingBox.left = boundingBox.left;
    }
    // Top
    if (_currentNode.boundingBox.top > boundingBox.top)
    {
        // Update the height
        if (_currentNode.boundingBox.height > 0)
        {
            _currentNode.boundingBox.height += (_currentNode.boundingBox.top - boundingBox.top);
        }
        // Update the node BB left
        _currentNode.boundingBox.top = boundingBox.top;
    }

    // Width
    const float width = (boundingBox.left + boundingBox.width) - _currentNode.boundingBox.left;
    _currentNode.boundingBox.width = std::max(_currentNode.boundingBox.width, width);
    // Height
    const float height = (boundingBox.top + boundingBox.height) - _currentNode.boundingBox.top;
    _currentNode.boundingBox.height = std::max(_currentNode.boundingBox.height, height);
}

void BVH::RecursiveSearch(Collider& _searchObject, const Node& _currentNode)
{
    // If the search object is not within the current node bounding box, then return
    if (!AABBCollision(_currentNode.boundingBox, _searchObject.GetBoundingBox()) )
    {
        return;
    }
    if (_currentNode.childIndex != 0)
    {
        // Therefore we are not at a leaf node
        // Keep using recursion
        //Node& childA = m_nodeVec[_currentNode.childIndex];
        RecursiveSearch(_searchObject, m_nodeVec[_currentNode.childIndex]);

        //Node& childB = m_nodeVec[_currentNode.childIndex + 1];
        RecursiveSearch(_searchObject, m_nodeVec[_currentNode.childIndex + 1]);
        return;
    }
    // We are now at a leaf node
    // Check all the colliders within this node. Returns the index of gameobjects it collided with
    for (uint32_t index = _currentNode.objectIndex; index < _currentNode.objectIndex + _currentNode.objectCount; index++)
    {
        GameObject& currentGameObject = m_gameObjects[index];
        if (AABBCollision(currentGameObject.GetCollider().GetBoundingBox(), _searchObject.GetBoundingBox()))
        {
            // Object hit. Note down the index of collision
            m_collidedObjectsQueue.emplace_back(index);
        }
    }
}
