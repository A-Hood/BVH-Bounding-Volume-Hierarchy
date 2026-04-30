#include "BoundingVolumeHierarchy.h"

#include <chrono>
#include <iostream>

// Should be changed to not require colliders as parameters
void BVH::Generate() {
#if _BVHDEBUG
	auto t1 = std::chrono::high_resolution_clock::now();
#endif

	m_masterNode = new Node();
	m_masterNode->DefineColliders(m_colliders);
	CreateNewNode(m_masterNode, 1, 18);

#if _BVHDEBUG
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> time = t2 - t1;
	std::cout << "Time to create BVH: " << std::to_string(time.count()) << "ms" << std::endl;
#endif
}

SearchResult BVH::Search(FloatRect _rect) {
	m_collisionQueue.clear();
	m_collisionNodesQueue.clear();

    // Traverse through the bvh, then check objects within that node
    auto t1 = std::chrono::high_resolution_clock::now();
    RecursiveSearch(_rect, m_masterNode); // start search at master node
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> bvhSearch = t2 - t1;

	// Set result of search
	SearchResult result;
	result.collisions = m_collisionQueue;
	result.nodes = m_collisionNodesQueue;
	result.timeTaken = bvhSearch.count();

	return result;
}

// AABB (non-rotated)
bool BVH::AABBCollision(FloatRect _boxA, FloatRect _boxB) {
	return _boxA.left < _boxB.left   + _boxB.width &&
		   _boxA.left + _boxA.width  > _boxB.left &&
		   _boxA.top  + _boxA.height > _boxB.top &&
		   _boxA.top  < _boxB.top    + _boxB.height;
}
// Seperating-Axis Theorem (for polygons)

FloatRect BVH::CalculateBoundingBox(const std::vector<Collider*>& nodeVector) {
	// Fill in the values of the back object in the current node
	float smallestX = nodeVector.back()->boundingBox.left;
	float largestX = nodeVector.back()->boundingBox.left + nodeVector.back()->boundingBox.width;

	float smallestY = nodeVector.back()->boundingBox.top;
	float largestY = nodeVector.back()->boundingBox.top + nodeVector.back()->boundingBox.height;

	// Find smallest and largest X and Y values
	for (Collider* object : nodeVector) {
		// Smallest X
		if (object->boundingBox.left < smallestX) {
			smallestX = object->boundingBox.left;
		}
		// Largest X
		if (object->boundingBox.left + object->boundingBox.width > largestX) {
			largestX = object->boundingBox.left + object->boundingBox.width;
		}
		// Smallest Y
		if (object->boundingBox.top < smallestY) {
			smallestY = object->boundingBox.top;
		}
		// Largest Y
		if (object->boundingBox.top + object->boundingBox.height > largestY) {
			largestY = object->boundingBox.top + object->boundingBox.height;
		}
	}

	return { smallestX, smallestY, largestX - smallestX, largestY - smallestY };
}

bool BVH::CheckXLongestSide(FloatRect boundingBox) {
	// True: X is the longest side
	// False: Y is the longest side
	return boundingBox.width >= boundingBox.height;
}

void BVH::AssignObjectSide(std::vector<Collider*>& leftSide, std::vector<Collider*>& rightSide, const Node* currentNode, float boundaryMidpoint, bool xIsLongestSide) {
	float objectMidpoint;

	for (Collider* object : currentNode->m_colliders)
	{
		// Calculate the midpoint of the object
		if (xIsLongestSide) {
			objectMidpoint = object->boundingBox.left + (object->boundingBox.width / 2);
		}
		else {
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
}

void BVH::CreateNewNode(Node* currentNode, size_t currentDepth, size_t maximumDepth) {
#if _BVHDEBUG
	// DEBUG
	currentNode->DefineDepth(currentDepth);
#endif

	// Calculate the bounding box
	FloatRect boundingBox = CalculateBoundingBox(currentNode->m_colliders);
	currentNode->DefineBounds(boundingBox);

	// Return if the number of game objects is 3 or less and it has reached the maximum depth
	if (currentNode->m_colliders.size() <= 3 || currentDepth >= maximumDepth)
	{
		return;
	}

	// Create empty vectors
	std::vector<Collider*> leftSide;
	std::vector<Collider*> rightSide;

	float boundaryMidpoint;		// Can represent either on the x or the y
	if (CheckXLongestSide(boundingBox))
	{
		// Width is the longest side
		boundaryMidpoint = boundingBox.left + (boundingBox.width / 2);
		AssignObjectSide(leftSide, rightSide, currentNode, boundaryMidpoint, true);
	}
	else
	{
		// Height is the longest side
		boundaryMidpoint = boundingBox.top + (boundingBox.height / 2);
		AssignObjectSide(leftSide, rightSide, currentNode, boundaryMidpoint, false);
	}

	Node* childA = new Node();
	currentNode->DefineChildA(childA);

	Node* childB = new Node();
	currentNode->DefineChildB(childB);

	// Define parents
	currentNode->childA->DefineParentNode(currentNode);
	currentNode->childB->DefineParentNode(currentNode);

	currentNode->childA->DefineColliders(leftSide);
	currentNode->childB->DefineColliders(rightSide);

	CreateNewNode(currentNode->childA, currentDepth + 1, maximumDepth);
	CreateNewNode(currentNode->childB, currentDepth + 1, maximumDepth);
}

void BVH::RecursiveSearch(FloatRect searchRect, Node* currentNode) {
	// If the searchRect is not within this current node, do not proceed
	if (!AABBCollision(searchRect, currentNode->boundingBox))
	{
		return;
	}
	// If the child nodes are a nullptr, therefore this is the leaf node.
	// Check objects within the leaf node
	// Else if this is not a leaf node, recurse
	if (currentNode->childA != nullptr && currentNode->childB != nullptr)
	{
		RecursiveSearch(searchRect, currentNode->childA);
		RecursiveSearch(searchRect, currentNode->childB);
		return;
	}
	// If this is not a nullptr, the searchRect is within this node, and there are two or fewer objects with this node, then write it down
	// Check collisions with object inside of node
	// If node contains a non-static collider, push it to queue to be recalculated
	bool nodeHasDynamic = false;
	for (Collider* col : currentNode->m_colliders)
	{
		if (AABBCollision(searchRect, col->boundingBox))
		{
			m_collisionQueue.emplace_back(col);
			if (!col->isStatic) {
				nodeHasDynamic = true;
			}
		}
	}
	if (nodeHasDynamic) {
		m_collisionNodesQueue.emplace_back(currentNode);
	}
}

void BVH::RecalculateBounds(Node* currentNode) {
	FloatRect boundingBox = CalculateBoundingBox(currentNode->m_colliders);
	currentNode->DefineBounds(boundingBox);

	if (currentNode->previousNode != nullptr)
	{
		RecalculateBounds(currentNode->previousNode);
	}
}
