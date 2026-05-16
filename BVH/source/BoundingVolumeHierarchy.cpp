#include "BoundingVolumeHierarchy.h"

#include <algorithm>
#include <chrono>
#include <iostream>

#include "Application.h"

BVH::BVH(size_t _maxDepth)
{
    m_maximumDepth = _maxDepth;
}

BVH::~BVH()
{
    // TODO: Clean up the memory
}

void BVH::CreateColliderRef(const std::vector<Collider>& _colliderVecRef)
{
    m_colliders = _colliderVecRef;
    m_nodeVec.reserve((2 * m_colliders.size()) - 1);
}

void BVH::GenerateBVH()
{
    auto t1 = std::chrono::system_clock::now();
    // Create master node - Master node is the first node in the vector
    Node masterNode;
    masterNode.objectIndex = 0;
    masterNode.objectCount = m_colliders.size();

    InitialiseNodeBoundingBox(masterNode);
    // Find the size of the master node
    for (auto index = masterNode.objectIndex; index < masterNode.objectIndex + masterNode.objectCount; index++)
    {
        GrowBoundingBox(masterNode, m_colliders[index]);
    }

    // Move into the vector
    m_nodeVec.reserve(8);
    m_nodeVec.emplace_back(masterNode);

    CreateNewNode(masterNode, 0);

    auto t2 = std::chrono::system_clock::now();
    std::chrono::duration<float, std::milli> time = t2 - t1;
    LOG("Time to create in ms: " + std::to_string(time.count()));

}

void BVH::DrawBVH(sf::RenderTarget& _target, size_t _currentDepth)
{
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
}

void BVH::InitialiseNodeBoundingBox(Node& _currentNode) const
{
    _currentNode.boundingBox.left = m_colliders[_currentNode.objectIndex].boundingBox.left;
    _currentNode.boundingBox.top = m_colliders[_currentNode.objectIndex].boundingBox.top;
}

void BVH::CreateNewNode(Node& _currentNode, size_t _currentDepth)
{
    _currentDepth++;
    if (_currentNode.objectCount <= m_maxObjectsInLeafNode || _currentDepth >= m_maximumDepth)
    {
        // Do not continue as we have hit the max size
        return;
    }
    // Split the current node bounding box
    sf::Vector2f result = ChooseSplit(_currentNode);
    int splitAxis = static_cast<int>(result.x);
    float splitPosition = result.y;

    // Store where the starting position of the children are
    // This will technically be the index just after the parent index
    _currentNode.childIndex = m_nodeVec.size();
    //size_t parentIndex = m_nodeVec.size() - 1;

    // Create child nodes
    Node childA;
    childA.objectIndex = _currentNode.objectIndex;
    childA.currentDepth = _currentDepth;

    Node childB;
    childB.objectIndex = _currentNode.objectIndex;
    childB.currentDepth = _currentDepth;

    for (auto index = _currentNode.objectIndex; index < _currentNode.objectIndex + _currentNode.objectCount; index++)
    {
        bool isSideA = m_colliders[index].GetObjectCentre()[splitAxis] < splitPosition;
        Node& currentChild = isSideA ? childA : childB;
        // Changes the size of the bounding box of the child node using the current collider
        GrowBoundingBox(currentChild, m_colliders[index]);
        currentChild.objectCount++;

        if (isSideA)
        {
            uint32_t swap = currentChild.objectIndex + currentChild.objectCount - 1;
            std::swap(m_colliders[index], m_colliders[swap]);
            childB.objectIndex++;
        }
    }
    m_nodeVec.emplace_back(childA);
    m_nodeVec.emplace_back(childB);

    CreateNewNode(childA, _currentDepth);
    CreateNewNode(childB, _currentDepth);
}

sf::Vector2f BVH::ChooseSplit(const Node& _currentNode) const
{
    // X returns the splitAxis
    // Y returns the splitPosition
    if (IsXLongestSide(_currentNode))
    {
        // X is the longest
        return {0, _currentNode.boundingBox.left + (_currentNode.boundingBox.width / 2.f)};
    }
    return {1, _currentNode.boundingBox.top + (_currentNode.boundingBox.height / 2.f)};
}

void BVH::GrowBoundingBox(Node& _currentNode, const Collider& _collider) const
{
    if (_currentNode.boundingBox.width <= 0 || _currentNode.boundingBox.height <= 0)
    {
        // We know this node is new.
        // Therefore, we must set the position and size to the current collider
        // Return after
        _currentNode.boundingBox.left = _collider.boundingBox.left;
        _currentNode.boundingBox.top = _collider.boundingBox.top;
        _currentNode.boundingBox.width = _collider.boundingBox.width;
        _currentNode.boundingBox.height = _collider.boundingBox.height;
        return;
    }
    // Left
    if (_currentNode.boundingBox.left > _collider.boundingBox.left)
    {
        // Update the width
        if (_currentNode.boundingBox.width > 0)
        {
            _currentNode.boundingBox.width += (_currentNode.boundingBox.left - _collider.boundingBox.left);
        }
        // Update the node BB left
        _currentNode.boundingBox.left = _collider.boundingBox.left;
    }
    // Top
    if (_currentNode.boundingBox.top > _collider.boundingBox.top)
    {
        // Update the height
        if (_currentNode.boundingBox.height > 0)
        {
            _currentNode.boundingBox.height += (_currentNode.boundingBox.top - _collider.boundingBox.top);
        }
        // Update the node BB left
        _currentNode.boundingBox.top = _collider.boundingBox.top;
    }

    // Width
    const float width = (_collider.boundingBox.left + _collider.boundingBox.width) - _currentNode.boundingBox.left;
    _currentNode.boundingBox.width = std::max(_currentNode.boundingBox.width, width);
    // Height
    const float height = (_collider.boundingBox.top + _collider.boundingBox.height) - _currentNode.boundingBox.top;
    _currentNode.boundingBox.height = std::max(_currentNode.boundingBox.height, height);
}

bool BVH::IsXLongestSide(const Node& _currentNode) const
{
    return _currentNode.boundingBox.width > _currentNode.boundingBox.height;
}
