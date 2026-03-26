#include "BoundingVolumeHierarchy.h"

#include <chrono>
#include <iostream>
#include "GameObject.h"

// Should be changed to not require colliders as parameters
void BVH::Generate() {
#if _BVHDEBUG
	auto t1 = std::chrono::high_resolution_clock::now();
#endif

	Node* masterNode = new Node();
	m_nodes.emplace_back(masterNode);
	masterNode->DefineObjects(m_ptrGameObjects);
	CreateNewNode(masterNode, 1, 18);

#if _BVHDEBUG
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> time = t2 - t1;
	std::cout << "Time to create BVH: " << std::to_string(time.count()) << "ms" << std::endl;
#endif
}

std::vector<Node*>& BVH::GetNodes()
{
	return m_nodes;
}

void BVH::AddGameObject(GameObject* collider)
{
	m_ptrGameObjects.emplace_back(collider);
}


SearchResult BVH::Search(const GameObject& targetObject) {
	m_collisionQueue.clear();

    // Traverse through the bvh, then check objects within that node
    auto t1 = std::chrono::high_resolution_clock::now();
    //RecursiveSearch(targetObject, m_nodes[0]); // start search at master node
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> bvhSearch = t2 - t1;

	// Set result of search
	SearchResult result;
	result.collisions = m_collisionQueue;
	result.timeTaken = bvhSearch.count();

	return result;
}

// Seperating-Axis Theorem (for polygons)

sf::FloatRect BVH::CalculateBoundingBox(const std::vector<GameObject*>& nodeVector) {
	// Fill in the values of the back object in the current node
	float smallestX = nodeVector.back()->GetPosition().x;
	float largestX = nodeVector.back()->GetPosition().x + nodeVector.back()->GetSize().x;

	float smallestY = nodeVector.back()->GetPosition().y;
	float largestY = nodeVector.back()->GetPosition().y + nodeVector.back()->GetSize().y;

	// Find smallest and largest X and Y values
	for (GameObject* object : nodeVector) {
		// Smallest X
		if (object->GetPosition().x < smallestX) {
			smallestX = object->GetPosition().x;
		}
		// Largest X
		if (object->GetPosition().x + object->GetSize().x > largestX) {
			largestX = object->GetPosition().x + object->GetSize().x;
		}
		// Smallest Y
		if (object->GetPosition().y < smallestY) {
			smallestY = object->GetPosition().y;
		}
		// Largest Y
		if (object->GetPosition().y + object->GetSize().y > largestY) {
			largestY = object->GetPosition().y + object->GetSize().y;
		}
	}

	return { smallestX, smallestY, largestX - smallestX, largestY - smallestY };
}

bool BVH::CheckXLongestSide(const sf::FloatRect& boundingBox) {
	// True: X is the longest side
	// False: Y is the longest side
	return boundingBox.width >= boundingBox.height;
}

void BVH::AssignObjectSide(std::vector<GameObject*>& leftSide, std::vector<GameObject*>& rightSide, const Node* currentNode, float boundaryMidpoint, bool xIsLongestSide) {
	float objectMidpoint;

	for (GameObject* object : currentNode->m_colliders)
	{
		// Calculate the midpoint of the object
		if (xIsLongestSide) {
			objectMidpoint = object->GetPosition().x + (object->GetSize().x / 2);
		}
		else {
			objectMidpoint = object->GetPosition().y + (object->GetSize().y / 2);
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
	sf::FloatRect boundingBox = CalculateBoundingBox(currentNode->m_colliders);
	currentNode->DefineBounds(boundingBox);

	// Return if the number of game objects is 3 or less and it has reached the maximum depth
	if (currentNode->m_colliders.size() <= 3 || currentDepth >= maximumDepth)
	{
		return;
	}

	// Create empty vectors
	std::vector<GameObject*> leftSide;
	std::vector<GameObject*> rightSide;

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
	m_nodes.emplace_back(childA);
	currentNode->DefineChildA(childA);

	Node* childB = new Node();
	m_nodes.emplace_back(childB);
	currentNode->DefineChildB(childB);

	// Define parents
	currentNode->childA->DefineParentNode(currentNode);
	currentNode->childB->DefineParentNode(currentNode);

	currentNode->childA->DefineObjects(leftSide);
	currentNode->childB->DefineObjects(rightSide);

	CreateNewNode(currentNode->childA, currentDepth + 1, maximumDepth);
	CreateNewNode(currentNode->childB, currentDepth + 1, maximumDepth);
}

void BVH::RecursiveSearch(const GameObject& targetObject, Node* currentNode) {
	// If the searchRect is not within this current node, do not proceed
	if (!targetObject.CollideWith(currentNode))
	{
		return;
	}
	// If the child nodes are a nullptr, therefore this is the leaf node.
	// Check objects within the leaf node
	// Else if this is not a leaf node, recurse
	if (currentNode->childA != nullptr && currentNode->childB != nullptr)
	{
		RecursiveSearch(targetObject, currentNode->childA);
		RecursiveSearch(targetObject, currentNode->childB);
		return;
	}
	// If this is not a nullptr, the searchRect is within this node, and there are 3 or fewer objects with this node, then write it down
	// Check collisions with object inside of node
	for (GameObject* gameObject : currentNode->m_colliders)
	{
		if (targetObject.CollideWith(gameObject))
		{
			m_collisionQueue.emplace_back(gameObject);
		}

	}

}

void BVH::RecalculateBounds(Node* currentNode) {
	sf::FloatRect boundingBox = CalculateBoundingBox(currentNode->m_colliders);
	currentNode->DefineBounds(boundingBox);

	if (currentNode->previousNode != nullptr)
	{
		RecalculateBounds(currentNode->previousNode);
	}
}
