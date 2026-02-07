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

		std::cout << "Created bbVisual for " << _name << std::endl;
	}

	bool operator<(const GameObject& externalObj)
	{
		return this->boundingBox.left < externalObj.boundingBox.left;
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

	Node* previousNode = nullptr;
	Node* childA = nullptr;
	Node* childB = nullptr;

	std::vector<GameObject*> gameObjects;
	FloatRect boundingBox;
	sf::RectangleShape bbVisual;
};

std::vector<GameObject> gameObjects;
std::vector<Node*> bvh;


// TODO: Will be removed, only for debug purposes
/* Ignores bvh and manually checks all the collisions with every GameObject
 * Useful to check if the bvh is working correctly
 */
std::vector<GameObject*> tempCollisions;

FloatRect birdObject = {48, 48, 32, 32};
std::vector<Node*> collidedNodes;		// Each bird in angry birds will have this
std::vector<GameObject*> collidedObjects;

// Example of GameObjects within an application
void CreateGameObjects()
{
	// Creation of example obbjects
	gameObjects.emplace_back("circle", FloatRect(0, 0, 64, 64));
	gameObjects.emplace_back("chair", FloatRect(64, 0, 64, 64));
	gameObjects.emplace_back("dino", FloatRect(128, 0, 64, 64));
	gameObjects.emplace_back("obama", FloatRect(192, 0, 64, 64));
	gameObjects.emplace_back("chicken", FloatRect(0, 64, 64, 64));
	gameObjects.emplace_back("jockey", FloatRect(64, 64, 64, 64));
	gameObjects.emplace_back("frog", FloatRect(128, 64, 64, 64));
	gameObjects.emplace_back("shark", FloatRect(192, 64, 64, 64));
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

// BVH Stuff ------------------------------------------------------------------------------------------------------------------------

void OrganiseGameObjects()
{
	std::sort(gameObjects.begin(), gameObjects.end());
}

std::vector<GameObject*> ConvertVectorType(std::vector<GameObject>& gameObjectVector)
{
	std::vector<GameObject*> newVector;
	for (GameObject& object : gameObjectVector)
	{
		newVector.push_back(&object);
	}
	return newVector;
}

void CreateNewNode(Node* currentNode)
{
	// End node creation if the number of objects in the current node is 2 or less
	if (currentNode->gameObjects.size() <= 2)
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

	// Divide and conqour
	size_t midPoint = currentNode->gameObjects.size() / 2;
	std::vector<GameObject*> leftSide = {currentNode->gameObjects.begin(), currentNode->gameObjects.begin() + midPoint};
	std::vector<GameObject*> rightSide = {currentNode->gameObjects.begin() + midPoint, currentNode->gameObjects.end()};

	// Define objects in the next node
	currentNode->childA->DefineGameObjects(leftSide);
	currentNode->childB->DefineGameObjects(rightSide);

	// Recurse to child nodes
	CreateNewNode(currentNode->childA);
	CreateNewNode(currentNode->childB);
}

void CalculateNodeBounds(Node* currentNode)
{
	// We know the smallest and largest x as those are at the start and end of the vector
	float smallestX = currentNode->gameObjects.front()->boundingBox.left;
	float largestX = currentNode->gameObjects.back()->boundingBox.left + currentNode->gameObjects.back()->boundingBox.width;

	// Ensure there are default values
	float smallestY = currentNode->boundingBox.top;
	float largestY = currentNode->boundingBox.top + currentNode->boundingBox.height;

	// Find smallest and largest Y values
	for (GameObject* object : currentNode->gameObjects)
	{
		smallestY = std::min(object->boundingBox.top, smallestY);
		largestY = std::max(object->boundingBox.top + object->boundingBox.height, largestY);
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
	 * 7. Left side of midpoint goes to childA, while right of midpoint goes to childB - done
	 * 8. Repeat steps 4 to 8 using recursion until the number of gameObjects in that node is 2 or less - done
	 * 9. Calculate the bounds of all nodes using the gameObjects
	 */
	OrganiseGameObjects();

	// Create master node
	Node* masterNode = new Node();
	bvh.emplace_back(masterNode);
	masterNode->DefineGameObjects(ConvertVectorType(gameObjects));

	// Start creating bvh
	CreateNewNode(masterNode);

	// Calculate the bounds of all the nodes
	CalculateNodeBounds(masterNode);

	LOG("Number of nodes: " + std::to_string(bvh.size()))

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


int main()
{
	/* Seed random */
	srand(time(0));

	// Creation of BVH and GameObjects
	CreateGameObjects();
	CreateBVH();

	// Check all of the collisions
	//CheckCollison(birdObject);

	auto t1 = std::chrono::high_resolution_clock::now();
	// Traverse through the bvh, then check objects within that node
	//RecursiveSearchBVH(birdObject, &bvh[0]);
	//CheckCollisionsWithinNodes(birdObject);

	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> time = t2 - t1;
	bvhRecursive_timeInMs += time.count();

	// Print out objects hit by traversing bvh
	for (GameObject* object : collidedObjects) 
	{
		std::cout << "Object collided with: " << object->name << "\n";
	}

	std::cout << "Size of Full Search collisionQueue: " << tempCollisions.size() << std::endl;
	std::cout << "Full Search time to complete : " << fullSearch_timeInMs << "ms" << std::endl;
	std::cout << "Size of BVH Traverse collisionQueue: " << collidedNodes.size() << std::endl;
	std::cout << "BVH Traverse time to complete : " << bvhRecursive_timeInMs << "ms" << std::endl;

	sf::RenderWindow window(sf::VideoMode({ APP_SETTINGS.SCREEN_WIDTH, APP_SETTINGS.SCREEN_HEIGHT }), APP_SETTINGS.APPLICATION_NAME);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();

		/* Objects Visualisation */
		for (auto go : gameObjects) {
		    window.draw(go.bbVisual);
		}
		/* BVH Visualisation */
		for (auto node : bvh) {
			window.draw(node->bbVisual);
		}

		window.display();
	}

	return 0;
}