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
	void CreateLink(Node* _previousNode, Node* _childA, Node* _childB)
	{
		previousNode = _previousNode;
		childA = _childA;
		childB = _childB;
	}

	Node* previousNode = nullptr;
	Node* childA = nullptr;
	Node* childB = nullptr;

	std::vector<GameObject*> gameObjects;
	FloatRect boundingBox;
	sf::RectangleShape bbVisual;
};

std::vector<GameObject> gameObjects;
std::vector<Node> bvh;


// TODO: Will be removed, only for debug purposes
/* Ignores bvh and manually checks all the collisions with every GameObject
 * Useful to check if the bvh is working correctly
 */
std::vector<GameObject*> tempCollisions;

FloatRect birdObject = {48, 48, 32, 32};
std::vector<Node*> collidedNodes;		// Each bird in angry birds will have this

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

std::vector<GameObject*> CalculateObjectsWithinNode(FloatRect boundingBox)
{
	std::vector<GameObject*> tempVector;
	// Go through each game object and check if it lies within the specified bounding box
	for (GameObject& object : gameObjects)
	{
		if (!BoxBoxCollision(boundingBox, object.boundingBox))
		{
			// Not within box
			continue;
		}
		tempVector.emplace_back(&object);
	}
	return tempVector;
}

void CreateBVH()
{
	// Creation of the bvh - will be automated
	bvh.emplace_back();		// Master = 0
	bvh.emplace_back();		// 1
	bvh.emplace_back();		// 2
	bvh.emplace_back();		// 3
	bvh.emplace_back();		// 4
	bvh.emplace_back();		// 5
	bvh.emplace_back();		// 6

	// Create the links between nodes - will be automated
	bvh[0].CreateLink(nullptr, &bvh[1], &bvh[2]);
	bvh[1].CreateLink(&bvh[0], &bvh[3], &bvh[5]);
	bvh[2].CreateLink(&bvh[0], &bvh[4], &bvh[6]);
	bvh[3].CreateLink(&bvh[1], nullptr, nullptr);
	bvh[4].CreateLink(&bvh[2], nullptr, nullptr);
	bvh[5].CreateLink(&bvh[1], nullptr, nullptr);
	bvh[6].CreateLink(&bvh[2], nullptr, nullptr);

	// Calculate the float rects for all the nodes in the bvh - will be automated
	bvh[0].DefineBounds(0, 0, 256, 128);				// Master = 0
	bvh[1].DefineBounds(0, 0, 128, 128);				// 1
	bvh[2].DefineBounds(128, 0, 128, 128);			// 2
	bvh[3].DefineBounds(0, 0, 128, 64);				// 3
	bvh[4].DefineBounds(128, 0, 128, 64);			// 4
	bvh[5].DefineBounds(0, 64, 128, 64);				// 5
	bvh[6].DefineBounds(128, 64, 128, 64);			// 6

	// Calculate what objects are given within the nodes
	for (Node& node : bvh)
	{
		std::vector<GameObject*> objectsWithinNode = CalculateObjectsWithinNode(node.boundingBox);
		node.DefineGameObjects(objectsWithinNode);
	}
}

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

/* Set this to node as of now due to BVH creation not adding gameobjects correctly */
void RecursiveSearchBVH(FloatRect searchRect, Node* currentNode) {

	
	// Do not continue if this node is a nullptr
	if (currentNode == nullptr)
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
	// Record the node that the searchRect is within
	if (BoxBoxCollision(searchRect, currentNode->boundingBox))
	{
		collidedNodes.push_back(currentNode);
	}
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
	CheckCollison(birdObject);

	auto t1 = std::chrono::high_resolution_clock::now();
	// Traverse through the bvh, then check objects within that node
	RecursiveSearchBVH(birdObject, &bvh[0]);
	CheckCollisionsWithinNodes(birdObject);

	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> time = t2 - t1;
	bvhRecursive_timeInMs += time.count();


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
			window.draw(node.bbVisual);
		}

		window.display();
	}

	return 0;
}