#include <algorithm>
#include <charconv>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <random>
#include <cstdlib>

#include <SFML/Graphics.hpp>

#define LOG(x) std::cout << x << std::endl;

struct Node;

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
	GameObject() = default;
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

	void SetPosition(sf::Vector2i position)
	{
		boundingBox.left = position.x;
		boundingBox.top = position.y;

		bbVisual.setPosition(boundingBox.left, boundingBox.top);
	}

	void IncrementPosition(sf::Vector2i position)
	{
		boundingBox.left += position.x;
		boundingBox.top += position.y;

		bbVisual.setPosition(boundingBox.left, boundingBox.top);
	}

	void DefineLeafNode(Node* _leafNode)
	{
		leafNode = _leafNode;
	}

	std::string name;
	FloatRect boundingBox;
	Node* leafNode = nullptr;
	sf::RectangleShape bbVisual;
};

struct Node {
	// Defines GameObjects within that node
	void DefineGameObjects(std::vector<GameObject*> gameObjectsInNode)
	{
		gameObjects = gameObjectsInNode;
	}
	// Bounds of the Node
	void DefineBounds(FloatRect _boundingBox)
	{
		boundingBox.left = _boundingBox.left;
		boundingBox.top = _boundingBox.top;
		boundingBox.width = _boundingBox.width;
		boundingBox.height = _boundingBox.height;

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
		if (currentDepth == depth || currentDepth == 0)
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

FloatRect birdObject = {1700, 0, 32, 32};
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

	gameObjects.emplace_back("circle", FloatRect(0, 0, 64, 64));
	gameObjects.emplace_back("chair", FloatRect(119, 0, 64, 64));
	gameObjects.emplace_back("dino", FloatRect(280 * 2.2f, 0, 64, 64));
	gameObjects.emplace_back("obama", FloatRect(395 * 3, 0, 64, 64));
	gameObjects.emplace_back("chicken", FloatRect(86, 128 * 1.2f, 64, 64));
	gameObjects.emplace_back("jockey", FloatRect(107, 128, 64, 64));
	gameObjects.emplace_back("frog", FloatRect(230, 128 * 3.17f, 64, 64));
	gameObjects.emplace_back("shark", FloatRect(297 * 3.1f, 128 * 4, 64, 64));

	/*
	gameObjects.emplace_back("thing", FloatRect(20, 20 + (90 * 1), 64, 64));
	gameObjects.emplace_back("thing", FloatRect(20, 20 + (90 * 2), 64, 64));
	gameObjects.emplace_back("thing", FloatRect(20, 20 + (90 * 3), 64, 64));
	gameObjects.emplace_back("thing", FloatRect(20, 20 + (90 * 4), 64, 64));
	gameObjects.emplace_back("thing", FloatRect(20, 20 + (90 * 5), 64, 64));
	*/
	/*
	for (int x = 0; x < 100000; x++)
	{
		int randomX = RandomGen(10, APP_SETTINGS.SCREEN_WIDTH - 74);
		int randomY = RandomGen(10, APP_SETTINGS.SCREEN_HEIGHT - 74);

		gameObjects.emplace_back("obj_" + std::to_string(x), FloatRect(randomX, randomY, 64, 64));
	}
	*/


	/*
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			gameObjects.emplace_back("thing", FloatRect(10 + (x * 90), 10 + (y * 90), 64, 64));
		}
	}
	*/
	
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


// Checks all the collisions in the scene to compare the results with the bvh
void CheckCollison(FloatRect collisionBox)
{
	for (GameObject& object : gameObjects)
	{
		if (BoxBoxCollision(collisionBox, object.boundingBox))
		{
			tempCollisions.push_back(&object);
		}
	}
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

FloatRect CalculateBoundingBox(const std::vector<GameObject*>& nodeVector)
{
	// Fill in the values of the back object in the current node
	float smallestX = nodeVector.back()->boundingBox.left;
	float largestX = nodeVector.back()->boundingBox.left + nodeVector.back()->boundingBox.width;

	float smallestY = nodeVector.back()->boundingBox.top;
	float largestY = nodeVector.back()->boundingBox.top + nodeVector.back()->boundingBox.height;

	// Find smallest and largest X and Y values
	for (GameObject* object : nodeVector)
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
	
	return {smallestX, smallestY, largestX - smallestX, largestY - smallestY};
}

bool CheckXLongestSide(FloatRect boundingBox)
{
	// True: X is the longest side
	// False: Y is the longest side
	return boundingBox.width >= boundingBox.height;
}
void AssignObjectSide(std::vector<GameObject*>& leftSide, std::vector<GameObject*>& rightSide, const Node* currentNode, float boundaryMidpoint, bool xIsLongestSide)
{
	float objectMidpoint;

	for (GameObject* object : currentNode->gameObjects)
	{
		// Calculate the midpoint of the object
		if (xIsLongestSide)
		{
			objectMidpoint = object->boundingBox.left + (object->boundingBox.width / 2);
		}
		else
		{
			objectMidpoint = object->boundingBox.top + (object->boundingBox.height / 2);
		}

		if (objectMidpoint < boundaryMidpoint)
		{
			// Object is moved to the left side
			leftSide.emplace_back(object);
		}
		else
		{
			// Object is moved to the right side
			rightSide.emplace_back(object);
		}
	}
}


void CreateNewNode(Node* currentNode, size_t currentDepth, size_t maximumDepth)
{
	// DEBUG
	currentNode->DefineDepth(currentDepth);
	// Calculate the bounding box
	FloatRect boundingBox = CalculateBoundingBox(currentNode->gameObjects);
	currentNode->DefineBounds(boundingBox);

	// Return if the number of game objects is 3 or less and it has reached the maximum depth
	if (currentNode->gameObjects.size() <= 3 || currentDepth >= maximumDepth)
	{
		// Before return, set each gameobject in this current node to be this current node
		for (GameObject* object : currentNode->gameObjects)
		{
			object->DefineLeafNode(currentNode);
		}
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
	bvh.emplace_back(childA);
	currentNode->DefineChildA(childA);

	Node* childB = new Node();
	bvh.emplace_back(childB);
	currentNode->DefineChildB(childB);

	// Define parents
	currentNode->childA->DefineParentNode(currentNode);
	currentNode->childB->DefineParentNode(currentNode);

	currentNode->childA->DefineGameObjects(leftSide);
	currentNode->childB->DefineGameObjects(rightSide);

	CreateNewNode(currentNode->childA, currentDepth + 1, maximumDepth);
	CreateNewNode(currentNode->childB, currentDepth + 1, maximumDepth);

}


void CreateBVH()
{
	/* 1. Create master node
	 * 2. All the objects in the list belong to the master node
	 * 3. Start the recursion
	 * 4. Go through the list to find the smallest x and y, largest x and y. Calculate the bounds 
	 * 5. Create two vectors called left and right. They are empty
	 * 6. Find the longest side, x or y
	 * 7. Start a foreach loop of the game objects
	 * 8. Calculate the midpoint of the object
	 * 9. If the x is the longest side: check if the x of the midpoint is less than the midpoint of the current box, therefore it will be on the left side. Else the object is on the right side. 
	 * 10. If on the left, move object to the left vector. Else move the object to the right vector
	 * 11. If the y is the longest side: check if the y of the midpoint is less than the midpoint of the current box, therefore it will be on the left side. Else the object is on the right side.
	 * 12. If on the left, move object to the left vector. Else move the object to the right vector
	 * 13. Create child node A and B
	 * 14. Left vec goes to childA, right vec goes to childB
	 */

	auto t1 = std::chrono::high_resolution_clock::now();

	Node* masterNode = new Node();
	bvh.emplace_back(masterNode);
	masterNode->DefineGameObjects(ConvertVectorType(gameObjects));
	CreateNewNode(masterNode, 1, 18);

	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> time = t2 - t1;
	LOG("Time to create BVH: " + std::to_string(time.count()) + "ms")
}

void RecalculateBVHBounds(Node* currentNode)
{
	FloatRect boundingBox = CalculateBoundingBox(currentNode->gameObjects);
	currentNode->DefineBounds(boundingBox);

	if (currentNode->previousNode != nullptr)
	{
		RecalculateBVHBounds(currentNode->previousNode);
	}
}


void RecursiveSearchBVH(FloatRect searchRect, Node* currentNode) 
{
	// If the searchRect is not within this current node, do not proceed
	if (!BoxBoxCollision(searchRect, currentNode->boundingBox))
	{
		return;
	}
	// If the child nodes are a nullptr, therefore this is the leaf node.
	// Check objects within the leaf node
	// Else if this is not a leaf node, recurse
	if (currentNode->childA != nullptr && currentNode->childB != nullptr)
	{
		RecursiveSearchBVH(searchRect, currentNode->childA);
		RecursiveSearchBVH(searchRect, currentNode->childB);
		return;
	}
	// If this is not a nullptr, the searchRect is within this node, and there are two or fewer objects with this node, then write it down
	// Check collisions with object inside of node
	for (GameObject* object : currentNode->gameObjects)
	{
		if (BoxBoxCollision(searchRect, object->boundingBox))
		{
			collidedObjects.emplace_back(object);
		}
	}
	
}

enum E_DigitalButton
{
	NONE = 0,
	PRESSED,
	ACTIVE,
	RELEASED
};


struct DigitalButton
{
	void SetButtonState(bool buttonHeld)
	{
		if (!buttonHeld)
		{
			m_buttonState = RELEASED;
			return;
		}
		if (m_buttonState == RELEASED)
		{
			m_buttonState = PRESSED;
			return;
		}
		if (m_buttonState == PRESSED)
		{
			m_buttonState = ACTIVE;
		}
	}

	E_DigitalButton GetButtonState()
	{
		return m_buttonState;
	}
	E_DigitalButton	m_buttonState = RELEASED;
};


const char* ConvertButtonStateToName(E_DigitalButton state)
{
	switch (state)
	{
		case PRESSED:
			return "PRESSED";
		case ACTIVE:
			return "ACTIVE";
		case RELEASED:
			return "RELEASED";
		case NONE:
		default:
			return "";
	}
}

DigitalButton leftMouse;
sf::Vector2i previousMousePos;

void MouseClickAction(const sf::Window& window)
{
	sf::Vector2i currentMousePos = sf::Mouse::getPosition(window);
	sf::Vector2i deltaMousePos = currentMousePos - previousMousePos;

	if (collidedObjects.empty())
	{
		return;
	}
	collidedObjects[0]->IncrementPosition(deltaMousePos);
	// Recalculate the bvh bounds
	RecalculateBVHBounds(collidedObjects[0]->leafNode);
}


size_t currentDepth = 0;
int main()
{
	/* Seed random */
	srand(time(0));

	// Creation of BVH and GameObjects
	CreateGameObjects();
	CreateBVH();

	// Check all of the collisions to compare against bvh search
	auto t1 = std::chrono::high_resolution_clock::now();
	CheckCollison(birdObject);
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> fullSearch = t2 - t1;
	fullSearch_timeInMs = fullSearch.count();


	// Traverse through the bvh, then check objects within that node
	t1 = std::chrono::high_resolution_clock::now();
	RecursiveSearchBVH(birdObject, bvh[0]);
	t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> bvhSearch = t2 - t1;
	bvhRecursive_timeInMs = bvhSearch.count();

	// DEBUG ONLY - manually check all collisions to compare with bvh
	for (GameObject* object : tempCollisions)
	{
		LOG("DEBUG, object collided with: ")
		LOG(object->name)
	}
	LOG("")
	// Print out objects hit by traversing bvh
	for (GameObject* object : collidedObjects) 
	{
		std::cout << "BVH, Object collided with: " << object->name << "\n";
	}
	collidedObjects.clear();

	LOG("")
	std::cout << "Full Search time to complete : " << fullSearch_timeInMs << "ms" << std::endl;
	std::cout << "BVH Traverse time to complete : " << bvhRecursive_timeInMs << "ms" << std::endl;

	sf::RenderWindow window(sf::VideoMode({ APP_SETTINGS.SCREEN_WIDTH, APP_SETTINGS.SCREEN_HEIGHT }), APP_SETTINGS.APPLICATION_NAME);
	window.setKeyRepeatEnabled(false);
	window.setFramerateLimit(60);

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
		for (const auto& go : gameObjects) {
		    window.draw(go.bbVisual);
		}
		/* BVH Visualisation */
		for (auto node : bvh) {
			window.draw(node->bbVisual);
			node->ChangeVisibility(currentDepth);
		}

		// Mouse
		leftMouse.SetButtonState(sf::Mouse::isButtonPressed(sf::Mouse::Left));

		if (leftMouse.GetButtonState() == E_DigitalButton::PRESSED)
		{
			sf::Vector2i mousePos = sf::Mouse::getPosition(window);

			t1 = std::chrono::high_resolution_clock::now();
			RecursiveSearchBVH(FloatRect(mousePos.x, mousePos.y, 1, 1), bvh[0]);
			t2 = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float, std::milli> bvhSearch = t2 - t1;


			t1 = std::chrono::high_resolution_clock::now();
			CheckCollison(FloatRect(mousePos.x, mousePos.y, 1, 1));
			t2 = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float, std::milli> fullSearch = t2 - t1;

			LOG("")
			LOG("Time for full search: " + std::to_string(fullSearch.count()) + "ms")
			LOG("Time for bvh search: " + std::to_string(bvhSearch.count()) + "ms")
		}

		if (leftMouse.GetButtonState() == E_DigitalButton::ACTIVE)
		{
			MouseClickAction(window);
		}
		previousMousePos = sf::Mouse::getPosition(window);

		if (leftMouse.GetButtonState() == E_DigitalButton::RELEASED)
		{
			collidedObjects.clear();
		}

		window.display();
	}

	return 0;
}