#include "BVHIterationTwo.h"

#include <algorithm>
#include <chrono>
#include <SFML/Graphics/RenderTarget.hpp>

#include "Application.h"
#include "NodePointer.h"

BVHIterationTwo::~BVHIterationTwo()
{
	// Clean the BVH
	m_colliders.clear();
	TraversalNodeDestroy(m_masterNode);
}

// --- Public functions ---------------------------------------------------------------------------------
void BVHIterationTwo::AddCollider(Collider* _collider)
{
	m_colliders.emplace_back(_collider);
}

// Should be changed to not require colliders as parameters
void BVHIterationTwo::Generate()
{
	// Creates the master node
	m_masterNode = new NodePointer();
	m_masterNode->m_nodeColliders = m_colliders;
	for (auto collider : m_masterNode->m_nodeColliders)
	{
		GrowBoundingBox(m_masterNode, collider);
	}
	// Start the recursion
	CreateNewNode(m_masterNode, 1);
}

void BVHIterationTwo::Draw(sf::RenderTarget& _target, size_t _currentDepth)
{
	RecursiveDraw(_target, m_masterNode, _currentDepth);
}

// DEBUG ONLY

// --- Private functions ---------------------------------------------------------------------------------

void BVHIterationTwo::CreateNewNode(NodePointer* _currentNode, size_t _currentDepth)
{
#if _BVHDEBUG
	// DEBUG
	_currentNode->m_currentDepth = _currentDepth;
#endif

	// Return if the number of game objects is m_maxObjectsInLeafNode or less
	// And it has reached the maximum depth
	if (_currentNode->m_nodeColliders.size() <= m_maxObjectsInLeafNode || _currentDepth >= m_maximumDepth)
	{
		return;
	}
	// Split the current node bounding box
	sf::Vector2i result = ChooseSplit(_currentNode);
	const int splitAxis = result.x;
	const int splitPosition = result.y;

	// Create child nodes
	NodePointer* childA = new NodePointer();
	NodePointer* childB = new NodePointer();

	// Define parents
	childA->m_parentNode = _currentNode;
	childB->m_parentNode = _currentNode;

	// Go through each collider of the current node
	for (auto collider : _currentNode->m_nodeColliders)
	{
		// Find if the box is on side A, if not then its on side B
		const bool isSideA = collider->GetCentreFromAxis(splitAxis) < splitPosition;
		NodePointer* currentChild = isSideA ? childA : childB;
		GrowBoundingBox(currentChild, collider);

		// Add collider to the current child
		currentChild->m_nodeColliders.emplace_back(collider);
	}
	// Assign nodes to the parent node
	_currentNode->m_childB = childB;
	_currentNode->m_childA = childA;

	CreateNewNode(_currentNode->m_childA, _currentDepth + 1);
	CreateNewNode(_currentNode->m_childB, _currentDepth + 1);
}

bool BVHIterationTwo::IsXLongestSide(const NodePointer* _currentNode) const
{
	return _currentNode->m_boundingBox.width > _currentNode->m_boundingBox.height;
}

sf::Vector2i BVHIterationTwo::ChooseSplit(const NodePointer* _currentNode)
{
	// X returns the splitAxis
	// Y returns the splitPosition
	if (IsXLongestSide(_currentNode))
	{
		// X is the longest
		return {0, static_cast<int>(_currentNode->m_boundingBox.left) + (static_cast<int>(_currentNode->m_boundingBox.width) / 2)};
	}
	return {1, static_cast<int>(_currentNode->m_boundingBox.top) + (static_cast<int>(_currentNode->m_boundingBox.height) / 2)};
}

void BVHIterationTwo::GrowBoundingBox(NodePointer* _currentNode, Collider* _collider)
{
	const auto& boundingBox = _collider->GetBoundingBox();
	if (_currentNode->m_boundingBox.width <= 0 || _currentNode->m_boundingBox.height <= 0)
	{
		// We know this node is new.
		// Therefore, we must set the position and size to the current collider
		// Return after
		_currentNode->m_boundingBox.left = boundingBox.left;
		_currentNode->m_boundingBox.top = boundingBox.top;
		_currentNode->m_boundingBox.width = boundingBox.width;
		_currentNode->m_boundingBox.height = boundingBox.height;
		return;
	}
	// Left
	if (_currentNode->m_boundingBox.left > boundingBox.left)
	{
		// Update the width
		if (_currentNode->m_boundingBox.width > 0)
		{
			_currentNode->m_boundingBox.width += (_currentNode->m_boundingBox.left - boundingBox.left);
		}
		// Update the node BB left
		_currentNode->m_boundingBox.left = boundingBox.left;
	}
	// Top
	if (_currentNode->m_boundingBox.top > boundingBox.top)
	{
		// Update the height
		if (_currentNode->m_boundingBox.height > 0)
		{
			_currentNode->m_boundingBox.height += (_currentNode->m_boundingBox.top - boundingBox.top);
		}
		// Update the node BB left
		_currentNode->m_boundingBox.top = boundingBox.top;
	}

	// Width
	const float width = (boundingBox.left + boundingBox.width) - _currentNode->m_boundingBox.left;
	_currentNode->m_boundingBox.width = std::max(_currentNode->m_boundingBox.width, width);
	// Height
	const float height = (boundingBox.top + boundingBox.height) - _currentNode->m_boundingBox.top;
	_currentNode->m_boundingBox.height = std::max(_currentNode->m_boundingBox.height, height);
}

void BVHIterationTwo::RecursiveDraw(sf::RenderTarget& _target, NodePointer* _currentNode, size_t _currentDepth)
{
#if _BVHDEBUG
	if (_currentDepth == 0 || _currentDepth == _currentNode->m_currentDepth)
	{
		sf::RectangleShape debugRect;
		debugRect.setPosition(_currentNode->m_boundingBox.left, _currentNode->m_boundingBox.top);
		debugRect.setSize({_currentNode->m_boundingBox.width, _currentNode->m_boundingBox.height});
		debugRect.setOutlineColor(sf::Color::Red);
		debugRect.setOutlineThickness(3);
		debugRect.setFillColor(sf::Color(0, 0, 0, 0));

		_target.draw(debugRect);
	}

	if (_currentNode->m_childA != nullptr)
	{
		RecursiveDraw(_target, _currentNode->m_childA, _currentDepth);
	}
	if (_currentNode->m_childB != nullptr)
	{
		RecursiveDraw(_target, _currentNode->m_childB, _currentDepth);
	}
#endif
}

void BVHIterationTwo::TraversalNodeDestroy(const NodePointer* _currentNode)
{
	if (_currentNode->m_childA != nullptr)
	{
		TraversalNodeDestroy(_currentNode->m_childA);
	}
	if (_currentNode->m_childB != nullptr)
	{
		TraversalNodeDestroy(_currentNode->m_childB);
	}
	// We are at a leaf node
	delete _currentNode;
}
//void BVHIterationTwo::RecalculateBounds(NodePointer* currentNode) {
//	FloatRect boundingBox = CalculateNodeBoundingBox(currentNode->m_colliders);
//	currentNode->DefineBounds(boundingBox);
//
//	if (currentNode->previousNode != nullptr)
//	{
//		RecalculateBounds(currentNode->previousNode);
//	}
//}
