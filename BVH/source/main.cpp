#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <cstdlib>

#include <SFML/Graphics.hpp>

#define LOG(x) std::cout << x << std::endl;

struct APPLICATION_SETTINGS {
	const uint16_t SCREEN_WIDTH = 1920;
	const uint16_t SCREEN_HEIGHT = 1080;
	const char* APPLICATION_NAME = "BVH Visualisation";
};
APPLICATION_SETTINGS APP_SETTINGS;

float fullSearch_timeInMs = 0.0f;
float bvhRecursive_timeInMs = 0.0f;

struct FloatRect {
	FloatRect() = default;
	FloatRect(float _left, float _top, float _width, float _height) {
		left = _left;
		top = _top;
		width = _width;
		height = _height;
	}

	float left = 0;
	float top = 0;
	float width = 0;
	float height = 0;
};

struct GameObject {
	GameObject(std::string _name, FloatRect _boundingBox) {
		name = _name;
		boundingBox = _boundingBox;

		/* SFML Specifics */
		bbVisual.setPosition(boundingBox.left, boundingBox.top);
		bbVisual.setSize({ _boundingBox.width, _boundingBox.height });

		int rR = rand() % 255;
		int rG= rand() % 255;
		int rB= rand() % 255;
		bbVisual.setFillColor(sf::Color(rR, rG, rB));
	}

	std::string name;
	FloatRect boundingBox;
	sf::RectangleShape bbVisual;
};

struct Node {
	Node() = default;

	// Defines GameObjects within that node
	void DefineGameObjects(std::vector<GameObject*> gameObjectsInNode)
	{
		gameObjects = gameObjectsInNode;
	}
	// Bounds of the Node
	void DefineBounds(float _left, float _top, float _width, float _height)
	{
		boundingBox.left = _left;
		boundingBox.top = _top;
		boundingBox.width = _width;
		boundingBox.height = _height;

		/* SFML Stuff */
		bbVisual.setPosition(boundingBox.left, boundingBox.top);
		bbVisual.setSize({ boundingBox.width, boundingBox.height });
		bbVisual.setOutlineColor(sf::Color::Red);
		bbVisual.setOutlineThickness(3);
		bbVisual.setFillColor(sf::Color(0, 0, 0, 0));
	}
	// Define the previous nodes and child nodes
	void DefineChildA(Node* _childA)
	{
		this->childA = _childA;
	}

	void DefineChildB(Node* _childB)
	{
		this->childB = _childB;
	}

	void DefineParentNode(Node* _parentNode)
	{
		this->previousNode = _parentNode;
	}

	void DefineDepth(size_t _depth)
	{
		this->depth = _depth;
	}

	void ChangeVisibility(size_t currentDepth)
	{
		if (currentDepth == depth)
		{
			bbVisual.setOutlineColor(sf::Color::Red);
		}
		else
		{
			bbVisual.setOutlineColor(sf::Color::Transparent);
		}
	}

	Node* previousNode = nullptr;
	Node* childA = nullptr;
	Node* childB = nullptr;

	std::vector<GameObject*> gameObjects;
	FloatRect boundingBox;

	// DEBUG
	sf::RectangleShape bbVisual;
	size_t depth = 1;

};

std::vector<GameObject> gameObjects;
std::vector<Node*> bvh;


// TODO: Will be removed, only for debug purposes
/* Ignores bvh and manually checks all the collisions with every GameObject
 * Useful to check if the bvh is working correctly
 */
std::vector<GameObject*> tempCollisions;

FloatRect birdObject = {90, 128, 32, 32};
std::vector<Node*> collidedNodes;		// Each bird in angry birds will have this
std::vector<GameObject*> collidedObjects;

int RandomGen(size_t minValue, size_t maxValue)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(minValue, maxValue);
	return dist(gen);
}

// Example of GameObjects within an application
void CreateGameObjects()
{
	// Creation of example objects
	/*
	gameObjects.emplace_back("circle", FloatRect(0, 0, 64, 64));
	gameObjects.emplace_back("chair", FloatRect(119, 0, 64, 64));
	gameObjects.emplace_back("dino", FloatRect(280 * 2.2f, 0, 64, 64));
	gameObjects.emplace_back("obama", FloatRect(395 * 3, 0, 64, 64));
	gameObjects.emplace_back("chicken", FloatRect(86, 128 * 1.2f, 64, 64));
	gameObjects.emplace_back("jockey", FloatRect(107, 128, 64, 64));
	gameObjects.emplace_back("frog", FloatRect(230, 128 * 3.17f, 64, 64));
	gameObjects.emplace_back("shark", FloatRect(297 * 3.1f, 128 * 4, 64, 64));
	*/
	/*
	gameObjects.emplace_back("thing", FloatRect(20, 20 + (90 * 1), 64, 64));
	gameObjects.emplace_back("thing", FloatRect(20, 20 + (90 * 2), 64, 64));
	gameObjects.emplace_back("thing", FloatRect(20, 20 + (90 * 5), 64, 64));
	gameObjects.emplace_back("thing", FloatRect(20, 20 + (90 * 3), 64, 64));
	gameObjects.emplace_back("thing", FloatRect(20, 20 + (90 * 4), 64, 64));
	*/

	for (int y = 0; y < 5; y++)
	{
		for (int x = 0; x < 5; x++)
		{
			int randomX = RandomGen(10, APP_SETTINGS.SCREEN_WIDTH - 74);
			int randomY = RandomGen(10, APP_SETTINGS.SCREEN_HEIGHT - 74);

			gameObjects.emplace_back("thing", FloatRect(randomX, randomY, 64, 64));
		}
	}
}

bool BoxBoxCollision(FloatRect boxA, FloatRect boxB)
{
	if (boxA.left < boxB.left + boxB.width &&
			boxA.left + boxA.width > boxB.left &&
			boxA.top + boxA.height > boxB.top &&
			boxA.top < boxB.top + boxB.height)
	{
		return true;
	}
	return false;
}


// DEBUG STUFF  ---------------------------------------------------------------------------------------------------------------------


// Will change, the function will pass in the vector of what's inside that bounding box of the bvh rather than all objects
void CheckCollison(FloatRect collisionBox)
{
	auto t1 = std::chrono::high_resolution_clock::now();
	for (GameObject& object : gameObjects)
	{
		if (BoxBoxCollision(collisionBox, object.boundingBox))
		{
			tempCollisions.push_back(&object);
		}
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> time = t2 - t1;
	fullSearch_timeInMs += time.count();
}

size_t printCounter = 0;
void PrintGameObjectNames(std::vector<GameObject*> myVec)
{
	LOG("-------------- Printing objects, counter: " + std::to_string(printCounter) + " --------------")
	for (GameObject* object : myVec)
	{
		LOG(object->name)
	}
	LOG("-------------- Printing objects end --------------")
	printCounter++;
}

// Sorting algorithms ----------------------------------------------------------------------------------------------------------------



bool InitSortComparison(const GameObject& objectA, const GameObject& objectB)
{
	return objectA.boundingBox.left < objectB.boundingBox.left;
}
void InitObjectSort()
{
	std::sort(gameObjects.begin(), gameObjects.end(), InitSortComparison);
}

bool VerticalComparison(const GameObject* objectA, const GameObject* objectB)
{
	return objectA->boundingBox.top < objectB->boundingBox.top;
}
bool HorizontalComparison(const GameObject* objectA, const GameObject* objectB)
{
	return objectA->boundingBox.left < objectB->boundingBox.left;
}

void BVHySort(std::vector<GameObject*>& unsortedVector)
{
	std::sort(unsortedVector.begin(), unsortedVector.end(), VerticalComparison);
}
void BVHxSort(std::vector<GameObject*>& unsortedVector)
{
	std::sort(unsortedVector.begin(), unsortedVector.end(), HorizontalComparison);
}





// BVH Stuff ------------------------------------------------------------------------------------------------------------------------

std::vector<GameObject*> ConvertVectorType(std::vector<GameObject>& gameObjectVector)
{
	std::vector<GameObject*> newVector;
	for (GameObject& object : gameObjectVector)
	{
		newVector.push_back(&object);
	}
	return newVector;
}

void CreateNewNode(Node* currentNode, size_t currentDepth)
{
	// DEBUG
	currentNode->DefineDepth(currentDepth);
	// End node creation if the number of objects in the current node is 3 or less
	if (currentNode->gameObjects.size() <= 3)
	{
		// This node is now a leaf node
		return;
	}

	Node* childA = new Node();		// ChildA
	bvh.emplace_back(childA);
	currentNode->DefineChildA(childA);

	Node* childB = new Node();		// ChildB
	bvh.emplace_back(childB);
	currentNode->DefineChildB(childB);

	// Assign parent node of both children
	currentNode->childA->DefineParentNode(currentNode);
	currentNode->childB->DefineParentNode(currentNode);

	// Debug
	BVHxSort(currentNode->gameObjects);
	// Find the  midpoint
	size_t midPoint = currentNode->gameObjects.size() / 2;
	if (currentNode->gameObjects[midPoint - 1]->boundingBox.left == currentNode->gameObjects[midPoint + 1]->boundingBox.left)
	{
		// Organise the objects from smallest y to largest y
		BVHySort(currentNode->gameObjects);
		// Recalculate the midpoint
		midPoint = currentNode->gameObjects.size() / 2;
	}

	std::vector<GameObject*> leftSide = {currentNode->gameObjects.begin(), currentNode->gameObjects.begin() + midPoint};
	std::vector<GameObject*> rightSide = {currentNode->gameObjects.begin() + midPoint, currentNode->gameObjects.end()};

	// Define objects in the next node
	currentNode->childA->DefineGameObjects(leftSide);
	currentNode->childB->DefineGameObjects(rightSide);

	// Recurse to child nodes
	CreateNewNode(currentNode->childA, currentDepth + 1);
	CreateNewNode(currentNode->childB, currentDepth + 1);
}

void CalculateNodeBounds(Node* currentNode)
{
	// Fill in the values of the back object in the current node
	float smallestX = currentNode->gameObjects.back()->boundingBox.left;
	float largestX = currentNode->gameObjects.back()->boundingBox.left + currentNode->gameObjects.back()->boundingBox.width;

	float smallestY = currentNode->gameObjects.back()->boundingBox.top;
	float largestY = currentNode->gameObjects.back()->boundingBox.top + currentNode->gameObjects.back()->boundingBox.height;

	// Find smallest and largest X and Y values
	for (GameObject* object : currentNode->gameObjects)
	{
		// Smallest X
		if (object->boundingBox.left < smallestX)
		{
			smallestX = object->boundingBox.left;
		}
		// Largest X
		if (object->boundingBox.left + object->boundingBox.width > largestX)
		{
			largestX = object->boundingBox.left + object->boundingBox.width;
		}
		// Smallest Y
		if (object->boundingBox.top < smallestY)
		{
			smallestY = object->boundingBox.top;
		}
		// Largest Y
		if (object->boundingBox.top + object->boundingBox.height > largestY)
		{
			largestY = object->boundingBox.top + object->boundingBox.height;
		}
	}

	currentNode->DefineBounds(smallestX, smallestY, largestX - smallestX, largestY - smallestY);

	if (currentNode->childA != nullptr)
	{
		CalculateNodeBounds(currentNode->childA);
	}
	if (currentNode->childB != nullptr)
	{
		CalculateNodeBounds(currentNode->childB);
	}

}


void CreateBVH()
{
	/* Steps to create a BVH
	 * 1. Organise the objects in the vector from smallest x to largest x - done
	 * 2. Create a master node which contains a vector of GameObject pointers - done
	 * 3. Start recursion by passing in the master node
	 * 4. Create two nodes - done
	 * 5. Assign the two new nodes as childA and childB of the current node - done
	 * 6. Find the midpoint of the current node vector - done
	 * 7. Check if either side of the midpoint has the same x value
	 *  Case false:
	 * 7.a.1. Left side of midpoint goes to childA, while right of midpoint goes to childB - done
	 * Case true:
	 * 7.b.1. Organise current vector from smallest y to largest y
	 * 7.b.2. Find the midpoint of the current vector
	 * 7.b.3. Left side of the midpoint goes to childA, while right of midpoint goes to childB
	 *
	 * 8. Repeat steps 4 to 8 using recursion until the number of gameObjects in that node is 3 or less - done
	 * 9. Calculate the bounds of all nodes using the gameObjects
	 */
	auto t1 = std::chrono::high_resolution_clock::now();
	InitObjectSort();

	// Create master node
	Node* masterNode = new Node();
	bvh.emplace_back(masterNode);
	masterNode->DefineGameObjects(ConvertVectorType(gameObjects));

	// Start creating bvh
	CreateNewNode(masterNode, 1);

	// Calculate the bounds of all the nodes
	CalculateNodeBounds(masterNode);

	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> time = t2 - t1;
	LOG("Time to create BVH: " + std::to_string(time.count()) + "ms")

}

/* Set this to node as of now due to BVH creation not adding gameobjects correctly */
void RecursiveSearchBVH(FloatRect searchRect, Node* currentNode) 
{
	// Do not continue if this node is a nullptr
	if (currentNode == nullptr)
	{
		return;
	}
	// If the searchRect is not within this current node, do not proceed
	if (!BoxBoxCollision(searchRect, currentNode->boundingBox))
	{
		return;
	}
	// Go to child nodes if there are more than 2 objects in this current node
	if (currentNode->gameObjects.size() > 2)
	{
		RecursiveSearchBVH(searchRect, currentNode->childA);
		RecursiveSearchBVH(searchRect, currentNode->childB);
		return;
	}
	// If this is not a nullptr, the searchRect is within this node, and there are two or fewer objects with this node, then write it down
	collidedNodes.emplace_back(currentNode);
	
}

void CheckCollisionsWithinNodes(FloatRect boundingBox)
{
	for (Node* node : collidedNodes)
	{
		// Check collisions with object inside of node
		for (GameObject* object : node->gameObjects)
		{
			if (BoxBoxCollision(boundingBox, object->boundingBox))
			{
				collidedObjects.emplace_back(object);
			}
		}
	}
}


size_t currentDepth = 0;
int main()
{
	/* Seed random */
	srand(time(0));

	// Creation of BVH and GameObjects
	CreateGameObjects();
	CreateBVH();

	// Check all of the collisions
	CheckCollison(birdObject);

	auto t1 = std::chrono::high_resolution_clock::now();
	// Traverse through the bvh, then check objects within that node
	RecursiveSearchBVH(birdObject, bvh[0]);
	CheckCollisionsWithinNodes(birdObject);

	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> time = t2 - t1;
	bvhRecursive_timeInMs += time.count();

	// DEBUG ONLY - manually check all collisions to compare with bvh
	for (GameObject* object : tempCollisions)
	{
		LOG("DEBUG, object collided with: " + object->name)
	}
	LOG("")
	// Print out objects hit by traversing bvh
	for (GameObject* object : collidedObjects) 
	{
		std::cout << "BVH, Object collided with: " << object->name << "\n";
	}
	LOG("")

	std::cout << "Size of Full Search collisionQueue: " << tempCollisions.size() << std::endl;
	std::cout << "Full Search time to complete : " << fullSearch_timeInMs << "ms" << std::endl;
	std::cout << "Size of BVH Traverse collisionQueue: " << collidedNodes.size() << std::endl;
	std::cout << "BVH Traverse time to complete : " << bvhRecursive_timeInMs << "ms" << std::endl;

	sf::RenderWindow window(sf::VideoMode({ APP_SETTINGS.SCREEN_WIDTH, APP_SETTINGS.SCREEN_HEIGHT }), APP_SETTINGS.APPLICATION_NAME);
	window.setKeyRepeatEnabled(false);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();


			if (event.type == sf::Event::KeyPressed)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				{
					currentDepth++;
					LOG(currentDepth)
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				{
					currentDepth--;
					LOG(currentDepth)
				}
			}
		}

		window.clear();

		/* Objects Visualisation */
		for (auto go : gameObjects) {
		    window.draw(go.bbVisual);
		}
		/* BVH Visualisation */
		for (auto node : bvh) {
			window.draw(node->bbVisual);
			node->ChangeVisibility(currentDepth);
		}

		window.display();
	}

	return 0;
}