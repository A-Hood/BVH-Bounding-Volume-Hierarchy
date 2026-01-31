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
	Node(std::vector<GameObject>& gameObjects, int _id)
	{
		ptrGameObjects = &gameObjects;
		id = _id;
	}

	void CreateLink(Node* _previousNode, Node* _childA, Node* _childB)
	{
		previousNode = _previousNode;
		childA = _childA;
		childB = _childB;
	}

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

	Node* previousNode = nullptr;
	Node* childA = nullptr;
	Node* childB = nullptr;

	int id;

	std::vector<GameObject>* ptrGameObjects;
	FloatRect boundingBox;
	sf::RectangleShape bbVisual;
};

std::vector<GameObject> gameObjects;
std::vector<Node> bvh;

FloatRect birdObject = {48, 48, 32, 32};

void CreateGameObjects()
{
	// Creation of example obbjects
	gameObjects.emplace_back("circle", FloatRect(0, 0, 64, 64));
	gameObjects.emplace_back("chair", FloatRect(64, 0, 64, 64));
	gameObjects.emplace_back("dino", FloatRect(128, 0, 64, 64));
	gameObjects.emplace_back("square", FloatRect(192, 0, 64, 64));
	gameObjects.emplace_back("chicken", FloatRect(0, 64, 64, 64));
	gameObjects.emplace_back("jockey", FloatRect(64, 64, 64, 64));
	gameObjects.emplace_back("frog", FloatRect(128, 64, 64, 64));
	gameObjects.emplace_back("shark", FloatRect(192, 64, 64, 64));
}

void CreateBVH()
{
	// Creation of the bvh - will be automated
	bvh.emplace_back(gameObjects, 0);		// Master = 0
	bvh.emplace_back(gameObjects, 1);		// 1
	bvh.emplace_back(gameObjects, 2);		// 2
	bvh.emplace_back(gameObjects, 3);		// 3
	bvh.emplace_back(gameObjects, 4);		// 4
	bvh.emplace_back(gameObjects, 5);		// 5
	bvh.emplace_back(gameObjects, 6);		// 6

	// Create the links between nodes
	bvh[0].CreateLink(nullptr, &bvh[1], &bvh[2]);
	bvh[1].CreateLink(&bvh[0], &bvh[3], &bvh[5]);
	bvh[2].CreateLink(&bvh[0], &bvh[4], &bvh[6]);
	bvh[3].CreateLink(&bvh[1], nullptr, nullptr);
	bvh[4].CreateLink(&bvh[2], nullptr, nullptr);
	bvh[5].CreateLink(&bvh[1], nullptr, nullptr);
	bvh[6].CreateLink(&bvh[2], nullptr, nullptr);

	// Calculate the float rects for all the nodes in the bvh - will be automated
	bvh[0].DefineBounds(0, 0, 256, 128);			// Master = 0
	bvh[1].DefineBounds(0, 0, 128, 128);			// 1
	bvh[2].DefineBounds(64, 0, 128, 128);			// 2
	bvh[3].DefineBounds(0, 0, 128, 64);				// 3
	bvh[4].DefineBounds(64, 0, 128, 64);			// 4
	bvh[5].DefineBounds(0, 64, 128, 64);			// 5
	bvh[6].DefineBounds(64, 64, 128, 64);			// 6
}

std::vector<GameObject*> goCollisionQueue;
// Will change, the function will pass in the vector of what's inside that bounding box of the bvh rather than all objects
void CheckCollison(FloatRect collisionBox)
{

	auto t1 = std::chrono::high_resolution_clock::now();
	for (GameObject& object : gameObjects)
	{
		if (collisionBox.left < object.boundingBox.left + object.boundingBox.width &&
			collisionBox.left + collisionBox.width >= object.boundingBox.left &&
			collisionBox.top + collisionBox.height >= object.boundingBox.top &&
			collisionBox.top < object.boundingBox.top + object.boundingBox.height)
		{
			goCollisionQueue.push_back(&object);
		}
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> time = t2 - t1;
	fullSearch_timeInMs += time.count();
}

bool CheckIfIntersecting(FloatRect bbox1, FloatRect bbox2) {
	if (bbox1.left < bbox2.left   + bbox2.width &&
		bbox1.left + bbox1.width  >= bbox2.left &&
		bbox1.top  + bbox1.height >= bbox2.top  &&
		bbox1.top  < bbox2.top    + bbox2.height ) 
	{
		// Collision has occured
		return true;
	}

	return false;
}

/* Set this to node as of now due to BVH creation not adding gameobjects correctly */
std::vector<Node*> collisionQueue;
void RecursiveSearchBVH(FloatRect searchRect,
	                    Node* node) {

	auto t1 = std::chrono::high_resolution_clock::now();

    if (CheckIfIntersecting(searchRect, node->boundingBox)) {
		/* Check if leaf */
        if (node->childA == nullptr && node->childB == nullptr) {
			collisionQueue.push_back(node);
			//LOG(std::string("Collision is occuring at node: " + std::to_string(node->id)))
        }
		/* If main node, search child nodes */
		else {
			RecursiveSearchBVH(searchRect, node->childA);
			RecursiveSearchBVH(searchRect, node->childB);
		}
    }
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> time = t2 - t1;
	bvhRecursive_timeInMs += time.count();
}




int main()
{
	/* Seed random */
	srand(time(0));

	CreateGameObjects();
	CreateBVH();
	//std::cout << "Size of BVH: " << bvh.size() << std::endl;

	CheckCollison(birdObject);
	RecursiveSearchBVH(birdObject, &bvh[0]);
	std::cout << "Size of Full Search collisionQueue: " << goCollisionQueue.size() << std::endl;
	std::cout << "Full Search time to complete : " << fullSearch_timeInMs << "ms" << std::endl;
	std::cout << "Size of BVH Traverse collisionQueue: " << collisionQueue.size() << std::endl;
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