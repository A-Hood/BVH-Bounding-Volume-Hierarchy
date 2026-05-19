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
	m_dynamicNodeQueue.clear();
	m_collidedObjectsQueue.clear();
	m_colliders.clear();

	TraversalNodeDestroy(m_masterNode);
}


// --- Public functions ---------------------------------------------------------------------------------

void BVH::AddCollider(Collider* _collider)
{
	m_colliders.push_back(_collider);
}

// Should be changed to not require colliders as parameters
void BVH::Generate()
{
	auto t1 = std::chrono::high_resolution_clock::now();

	// Creates the master node
	m_masterNode = new Node();
	m_masterNode->DefineColliders(m_colliders);
	// Start the recursion
	CreateNewNode(m_masterNode, 1);

	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> time = t2 - t1;
	std::cout << "Time to create BVH: " << std::to_string(time.count()) << "ms\n";
}

SearchResult BVH::SearchBVH(const FloatRect& _targetRect) {
	m_collidedObjectsQueue.clear();

    // Traverse through the bvh, then check objects within that node
    auto t1 = std::chrono::high_resolution_clock::now();
    RecursiveSearch(_targetRect, m_masterNode); // start search at master node
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> bvhSearch = t2 - t1;

	// Set result of search
	SearchResult result;
	result.searchTime = bvhSearch.count();
	result.numberCollidedObjects = m_collidedObjectsQueue.size();

	return result;
}

// DEBUG ONLY
Node* BVH::GetMasterNode() const
{
	return m_masterNode;
}

void BVH::DrawBVH(sf::RenderTarget& _target, Node* _currentNode, size_t _currentDepth) const
{
#if _BVHDEBUG
	_target.draw(_currentNode->bbVisual);
	_currentNode->ChangeVisibility(_currentDepth);

	if (_currentNode->childA != nullptr)
	{
		DrawBVH(_target, _currentNode->childA, _currentDepth);
	}
	if (_currentNode->childB != nullptr)
	{
		DrawBVH(_target, _currentNode->childB, _currentDepth);
	}
#endif
}


// --- Private functions ---------------------------------------------------------------------------------

bool BVH::AABBCollision(const FloatRect& _boxA, const FloatRect& _boxB) const
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
	currentNode->DefineDepth(currentDepth);
#endif

	// Return if the number of game objects is m_maxObjectsInLeafNode or less
	// And it has reached the maximum depth
	if (currentNode->m_colliders.size() <= m_maxObjectsInLeafNode || currentDepth >= m_maximumDepth)
	{
		return;
	}
	// Calculate the bounding box
	FloatRect boundingBox = CalculateNodeBoundingBox(currentNode->m_colliders);
	currentNode->DefineBounds(boundingBox);


	// Create child nodes
	Node* childA = new Node();
	currentNode->DefineChildA(childA);

	Node* childB = new Node();
	currentNode->DefineChildB(childB);

	// Define parents
	currentNode->childA->DefineParentNode(currentNode);
	currentNode->childB->DefineParentNode(currentNode);

	// Get the vectors from each child node
	auto& leftSide = currentNode->childA->GetCollders();
	auto& rightSide = currentNode->childB->GetCollders();
	// Reserve space to increase performance
	size_t reserveSize = currentNode->m_colliders.size() / 2;
	leftSide.reserve(reserveSize);
	rightSide.reserve(reserveSize);

	// Yeah not ideal
	float boundaryMidpoint;		// Can represent either on the x or the y
	if (IsXLongestSide(boundingBox))
	{
		boundaryMidpoint = boundingBox.left + (boundingBox.width / 2);
	}
	else
	{
		boundaryMidpoint = boundingBox.top + (boundingBox.height / 2);
	}
	AssignObjectSide(leftSide, rightSide, currentNode, boundaryMidpoint);

	CreateNewNode(currentNode->childA, currentDepth + 1);
	CreateNewNode(currentNode->childB, currentDepth + 1);
}

FloatRect BVH::CalculateNodeBoundingBox(const std::vector<Collider*>& nodeVector) const
{
	// Finds the smallest X and Y in the list of colliders
	// Also finds the largest X and Y in the list of colliders
	// Returns the position of the X and Y as well as the width and height of the bounding box

	// Fill in the values of the back object in the current node
	float smallestX = nodeVector.back()->boundingBox.left;
	float largestX = nodeVector.back()->boundingBox.left + nodeVector.back()->boundingBox.width;

	float smallestY = nodeVector.back()->boundingBox.top;
	float largestY = nodeVector.back()->boundingBox.top + nodeVector.back()->boundingBox.height;

	// Find smallest and largest X and Y values
	for (const auto object : nodeVector) {
		// Smallest X
		smallestX = std::min(object->boundingBox.left, smallestX);
		// Largest X
		largestX = std::max(object->boundingBox.left + object->boundingBox.width, largestX);
		// Smallest Y
		smallestY = std::min(object->boundingBox.top, smallestY);
		// Largest Y
		largestY = std::max(object->boundingBox.top + object->boundingBox.height, largestY);
	}
	return { smallestX, smallestY, largestX - smallestX, largestY - smallestY };
}

bool BVH::IsXLongestSide(const FloatRect& boundingBox) const
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
	bool nodeIsStatic = true;

	for (const auto object : currentNode->m_colliders)
	{
		// If any object is dynamic, then the node is a dynamic node
		if (!object->isStatic)
		{
			nodeIsStatic = false;
		}
		// Calculate the midpoint of the object
		if (IsXLongestSide(currentNode->boundingBox))
		{
			objectMidpoint = object->boundingBox.left + (object->boundingBox.width / 2);
		}
		else
		{
			objectMidpoint = object->boundingBox.top + (object->boundingBox.height / 2);
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
	DefineNodeType(currentNode, nodeIsStatic);
}
void BVH::DefineNodeType(Node* _currentNode, bool _nodeIsStatic)
{
	// Assigns the node type
	// Static node -> Does not need to be updated as objects do not move
	// Dynamic node -> Needs to be updated when objects move
	if (_nodeIsStatic)
	{
		return;
	}
	m_dynamicNodeQueue.emplace_back(_currentNode);
}

void BVH::TraversalNodeDestroy(const Node* _currentNode)
{
	if (_currentNode->childA != nullptr)
	{
		TraversalNodeDestroy(_currentNode->childA);
	}
	if (_currentNode->childB != nullptr)
	{
		TraversalNodeDestroy(_currentNode->childB);
	}
	// We are at a leaf node
	delete _currentNode;
}

void BVH::RecursiveSearch(const FloatRect& _searchRect, const Node* _currentNode) {
	// If the searchRect is not within this current node, do not proceed
	if (!AABBCollision(_searchRect, _currentNode->boundingBox))
	{
		return;
	}
	// If the child nodes are a nullptr, therefore this is the leaf node.
	// Check objects within the leaf node
	// Else if this is not a leaf node, recurse
	if (_currentNode->childA != nullptr)
	{
		RecursiveSearch(_searchRect, _currentNode->childA);
	}
	if (_currentNode->childB != nullptr)
	{
		RecursiveSearch(_searchRect, _currentNode->childB);
		return;
	}
	// If this is not a nullptr, the searchRect is within this node, then write it down
	// Check collisions with object inside of node
	for (const auto collider : _currentNode->m_colliders)
	{
		if (AABBCollision(_searchRect, collider->boundingBox))
		{
			m_collidedObjectsQueue.emplace_back(collider);
		}
	}
}
void BVH::RecalculateBounds(Node* currentNode) {
	FloatRect boundingBox = CalculateNodeBoundingBox(currentNode->m_colliders);
	currentNode->DefineBounds(boundingBox);

	if (currentNode->previousNode != nullptr)
	{
		RecalculateBounds(currentNode->previousNode);
	}
}
