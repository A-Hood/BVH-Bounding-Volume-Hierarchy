#include "BVHCacheLocality.h"

#include <SFML/Graphics/RectangleShape.hpp>

#include "NodeInt.h"
#include "Collider.h"
void BVHCacheLocality::CreateColliderRef(const std::vector<Collider>& _colliderVecRef)
{
    m_gameObjects = _colliderVecRef;
    m_nodeVec.reserve((2 * m_gameObjects.size()) - 1);
}

void BVHCacheLocality::Generate()
{
    NodeInt& masterNode = m_nodeVec.emplace_back();
    masterNode.objectIndex = 0;
    masterNode.objectCount = m_gameObjects.size();

    // Find the size of the master node
    for (auto index = masterNode.objectIndex; index < masterNode.objectIndex + masterNode.objectCount; index++)
    {
        GrowBoundingBox(masterNode, m_gameObjects[index]);
    }

    // Move into the vector
    CreateNewNode(masterNode, 0, 1);
}

void BVHCacheLocality::Draw(sf::RenderTarget& _target, size_t _currentDepth)
{
#if _BVHDEBUG
    for (NodeInt& currentNode : m_nodeVec)
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

void BVHCacheLocality::CreateNewNode(NodeInt& _currentNode, uint32_t parentIndex, size_t _currentDepth)
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
    NodeInt childA;
    childA.objectIndex = _currentNode.objectIndex;
    childA.parentIndex = parentIndex;

    NodeInt childB;
    childB.objectIndex = _currentNode.objectIndex;
    childB.parentIndex = parentIndex;

#if _BVHDEBUG
    // DEBUGGING ONLY
    childA.currentDepth = _currentDepth;
    childB.currentDepth = _currentDepth;
#endif

    for (auto index = _currentNode.objectIndex; index < _currentNode.objectIndex + _currentNode.objectCount; index++)
    {
        const bool isSideA = m_gameObjects[index].GetCentreFromAxis(splitAxis) < splitPosition;
        NodeInt& currentChild = isSideA ? childA : childB;
        // Changes the size of the bounding box of the child node using the current collider
        GrowBoundingBox(currentChild, m_gameObjects[index]);
        currentChild.objectCount++;

        if (isSideA)
        {
            const uint32_t swap = currentChild.objectIndex + currentChild.objectCount - 1;
            std::swap(m_gameObjects[index], m_gameObjects[swap]);
            childB.objectIndex++;
        }
    }
    m_nodeVec.emplace_back(childA);
    m_nodeVec.emplace_back(childB);

    CreateNewNode(m_nodeVec[currentNodeVecSize], currentNodeVecSize, _currentDepth + 1);
    CreateNewNode(m_nodeVec[currentNodeVecSize + 1], currentNodeVecSize + 1, _currentDepth + 1);
}

bool BVHCacheLocality::IsXLongestSide(const NodeInt& _currentNode) const
{
    return _currentNode.boundingBox.width > _currentNode.boundingBox.height;
}

sf::Vector2i BVHCacheLocality::ChooseSplit(const NodeInt& _currentNode) const
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

void BVHCacheLocality::GrowBoundingBox(NodeInt& _currentNode, Collider& _collider) const
{
    const auto& boundingBox = _collider.GetBoundingBox();
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
