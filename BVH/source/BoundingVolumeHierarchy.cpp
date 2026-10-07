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
	// Clean the BVH
	m_colliders.clear();
	TraversalNodeDestroy(m_masterNode);
}

// --- Public functions ---------------------------------------------------------------------------------
void BVH::AddCollider(Collider* _collider)
{
	m_colliders.emplace_back(_collider);
}

// Should be changed to not require colliders as parameters
void BVH::Generate()
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

SearchResult BVH::SearchForCollision(GameObject& _targetObject)
{
    // Traverse through the bvh, then check objects within that node
    auto t1 = std::chrono::high_resolution_clock::now();
    RecursiveSearch(_targetObject.GetCollider(), m_masterNode); // start search at master node
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> bvhSearch = t2 - t1;

	m_recursiveSearch.searchTime = bvhSearch.count();
	return m_recursiveSearch;
}

// DEBUG ONLY
Node* BVH::GetMasterNode() const
{
	return m_masterNode;
}

void BVH::Draw(sf::RenderTarget& _target, Node* _currentNode, size_t _currentDepth) const
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
		Draw(_target, _currentNode->m_childA, _currentDepth);
	}
	if (_currentNode->m_childB != nullptr)
	{
		Draw(_target, _currentNode->m_childB, _currentDepth);
	}
#endif
}


// --- Private functions ---------------------------------------------------------------------------------

bool BVH::AABBCollision(const sf::FloatRect& _boxA, const sf::FloatRect& _boxB) const
{
	return _boxA.left <= _boxB.left + _boxB.width &&
		_boxA.top <= _boxB.top + _boxB.height &&
		_boxA.left + _boxA.width >= _boxB.left &&
		_boxA.top + _boxA.height >= _boxB.top;
}

void BVH::CreateNewNode(Node* currentNode, size_t currentDepth)
{
#if _BVHDEBUG
	// DEBUG
	currentNode->m_currentDepth = currentDepth;
#endif

	// Return if the number of game objects is m_maxObjectsInLeafNode or less
	// And it has reached the maximum depth
	if (currentNode->m_nodeColliders.size() <= m_maxObjectsInLeafNode || currentDepth >= m_maximumDepth)
	{
		return;
	}
	// Calculate the bounding box
	Collider nodeBoundingBox = CalculateNodeBoundingBox(currentNode->m_nodeColliders);
	currentNode->m_boundingBox = nodeBoundingBox.GetBoundingBox();


	// Create child nodes
	Node* childA = new Node();
	currentNode->m_childA = childA;

	Node* childB = new Node();
	currentNode->m_childB = childB;

	// Define parents
	currentNode->m_childA->m_parentNode = currentNode;
	currentNode->m_childB->m_parentNode = currentNode;

	// Get the vectors from each child node
	auto& leftSide = currentNode->m_childA->m_nodeColliders;
	auto& rightSide = currentNode->m_childB->m_nodeColliders;
	// Reserve space to increase performance
	size_t reserveSize = currentNode->m_nodeColliders.size() / 2;
	leftSide.reserve(reserveSize);
	rightSide.reserve(reserveSize);

	// Yeah not ideal
	float boundaryMidpoint;		// Can represent either on the x or the y
	if (IsXLongestSide(currentNode->m_boundingBox))
	{
		boundaryMidpoint = currentNode->m_boundingBox.left + (currentNode->m_boundingBox.width / 2);
	}
	else
	{
		boundaryMidpoint = currentNode->m_boundingBox.top + (currentNode->m_boundingBox.height / 2);
	}
	AssignObjectSide(leftSide, rightSide, currentNode, boundaryMidpoint);

	CreateNewNode(currentNode->m_childA, currentDepth + 1);
	CreateNewNode(currentNode->m_childB, currentDepth + 1);
}

Collider BVH::CalculateNodeBoundingBox(const std::vector<Collider*>& _nodeVector) const
{
	// Finds the smallest X and Y in the list of colliders
	// Also finds the largest X and Y in the list of colliders
	// Returns the position of the X and Y as well as the width and height of the bounding box

	// Fill in the values of the back object in the current node
	const sf::FloatRect& boundingBox = _nodeVector.back()->GetBoundingBox();
	float smallestX = boundingBox.left;
	float largestX = boundingBox.left + boundingBox.width;

	float smallestY = boundingBox.top;
	float largestY = boundingBox.top + boundingBox.height;

	// Find smallest and largest X and Y values
	for (const auto object : _nodeVector) {
		const sf::FloatRect& objectBoundingBox = object->GetBoundingBox();
		// Smallest X
		smallestX = std::min(objectBoundingBox.left, smallestX);
		// Largest X
		largestX = std::max(objectBoundingBox.left + objectBoundingBox.width, largestX);
		// Smallest Y
		smallestY = std::min(objectBoundingBox.top, smallestY);
		// Largest Y
		largestY = std::max(objectBoundingBox.top + objectBoundingBox.height, largestY);
	}
	//return { smallestX, smallestY, largestX - smallestX, largestY - smallestY };
	Collider nodeBoundingBox;
	nodeBoundingBox.CreateBoundingBox({smallestX, smallestY}, {largestX - smallestX, largestY - smallestY});
	return nodeBoundingBox;
}

bool BVH::IsXLongestSide(const sf::FloatRect& boundingBox) const
{
	// True: X is the longest side
	// False: Y is the longest side
	return boundingBox.width >= boundingBox.height;
}

void BVH::  AssignObjectSide(std::vector<Collider*>& leftSide,
                           std::vector<Collider*>& rightSide,
                           Node* currentNode,
                           const float boundaryMidpoint)
{
	float objectMidpoint;

	for (const auto object : currentNode->m_nodeColliders)
	{
		// Calculate the midpoint of the object
		if (IsXLongestSide(currentNode->m_boundingBox))
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
void BVH::TraversalNodeDestroy(const Node* _currentNode)
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

void BVH::RecursiveSearch(Collider& _targetObject, const Node* _currentNode)
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
//void BVH::RecalculateBounds(Node* currentNode) {
//	FloatRect boundingBox = CalculateNodeBoundingBox(currentNode->m_colliders);
//	currentNode->DefineBounds(boundingBox);
//
//	if (currentNode->previousNode != nullptr)
//	{
//		RecalculateBounds(currentNode->previousNode);
//	}
//}
