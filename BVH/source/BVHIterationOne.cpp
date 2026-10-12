#include "BVHIterationOne.h"

#include <algorithm>
#include <chrono>
#include <iostream>

#include "Node.h"
#include "Application.h"

BVHIterationOne::~BVHIterationOne()
{
	// Clean the BVH
	m_colliders.clear();
	TraversalNodeDestroy(m_masterNode);
}

// --- Public functions ---------------------------------------------------------------------------------
void BVHIterationOne::AddCollider(Collider* _collider)
{
	m_colliders.emplace_back(_collider);
}

// Should be changed to not require colliders as parameters
void BVHIterationOne::Generate()
{
	auto t1 = std::chrono::high_resolution_clock::now();

	// Creates the master node
	m_masterNode = new Node();
	m_masterNode->m_nodeColliders = m_colliders;
	// Start the recursion
	CreateNewNode(m_masterNode, 1);

	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> time = t2 - t1;
	std::cout << "Time to create BVH: " << std::to_string(time.count()) << "ms\n";
}

// DEBUG ONLY
void BVHIterationOne::Draw(sf::RenderTarget& _target, size_t _currentDepth)
{
	RecursiveDraw(_target, m_masterNode, _currentDepth);
}

// --- Private functions ---------------------------------------------------------------------------------

void BVHIterationOne::CreateNewNode(Node* _currentNode, size_t _currentDepth)
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

	// Create child nodes
	Node* childA = new Node();
	Node* childB = new Node();

	// Define parents
	childA->m_parentNode = _currentNode;
	childB->m_parentNode = _currentNode;

	// Get the vectors from each child node
	auto& leftSide = childA->m_nodeColliders;
	auto& rightSide = childB->m_nodeColliders;

	// Reserve space to increase performance
	size_t reserveSize = _currentNode->m_nodeColliders.size() / 2;
	leftSide.reserve(reserveSize);
	rightSide.reserve(reserveSize);

	// Yeah not ideal
	float boundaryMidpoint;		// Can represent either on the x or the y
	if (IsXLongestSide(_currentNode))
	{
		boundaryMidpoint = _currentNode->m_boundingBox.left + (_currentNode->m_boundingBox.width / 2);
	}
	else
	{
		boundaryMidpoint = _currentNode->m_boundingBox.top + (_currentNode->m_boundingBox.height / 2);
	}

	// TODO: We can grow the bounding box then check which side the box is on
	AssignObjectSide(leftSide, rightSide, _currentNode, boundaryMidpoint);

	_currentNode->m_childB = childB;
	_currentNode->m_childA = childA;

	CreateNewNode(_currentNode->m_childA, _currentDepth + 1);
	CreateNewNode(_currentNode->m_childB, _currentDepth + 1);
}

bool BVHIterationOne::IsXLongestSide(const Node* _currentNode) const
{
	return _currentNode->m_boundingBox.width > _currentNode->m_boundingBox.height;
}

void BVHIterationOne::AssignObjectSide(std::vector<Collider*>& leftSide, std::vector<Collider*>& rightSide, Node* _currentNode, float boundaryMidpoint) const
{
	float objectMidpoint;
	for (const auto object : _currentNode->m_nodeColliders)
	{
		// Calculate the midpoint of the object
		if (IsXLongestSide(_currentNode))
		{
			objectMidpoint = object->GetBoundingBox().left + (object->GetBoundingBox().width / 2);
		}
		else
		{
			objectMidpoint = object->GetBoundingBox().top + (object->GetBoundingBox().height / 2);
		}

		if (objectMidpoint < boundaryMidpoint) {
			// Object is moved to the left side
			leftSide.emplace_back(object);
		}
		else {
			// Object is moved to the right side
			rightSide.emplace_back(object);
		}
	}
}

void BVHIterationOne::RecursiveDraw(sf::RenderTarget& _target, Node* _currentNode, size_t _currentDepth)
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

void BVHIterationOne::TraversalNodeDestroy(const Node* _currentNode)
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
//void BVHIterationOne::RecalculateBounds(Node* currentNode) {
//	FloatRect boundingBox = CalculateNodeBoundingBox(currentNode->m_colliders);
//	currentNode->DefineBounds(boundingBox);
//
//	if (currentNode->previousNode != nullptr)
//	{
//		RecalculateBounds(currentNode->previousNode);
//	}
//}
