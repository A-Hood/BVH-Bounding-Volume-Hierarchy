#include <algorithm>
#include <charconv>
#include <chrono>
#include <iostream>
#include <utility>
#include <vector>
#include <random>
#include <cstdlib>

#include <SFML/Graphics.hpp>

#define LOG(x) std::cout << x << std::endl;

/*
 * Create a "Dynamic" flag for each collider, store the dynamic colliders
 * as these are the colliders that will need to be checked against the others.
 * The static colliders will never move so do not need to update or need to check with other colliders.
 */

struct APPLICATION_SETTINGS {
	const uint16_t SCREEN_WIDTH = 1920;
	const uint16_t SCREEN_HEIGHT = 1080;
	const char* APPLICATION_NAME = "BVH Visualisation";
};
APPLICATION_SETTINGS APP_SETTINGS;

int RandomGen(size_t minValue, size_t maxValue)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(minValue, maxValue);
	return dist(gen);
}


#include "BoundingVolumeHierarchy.h"

// Debug Stuff
std::vector<Collider> colliders;
FloatRect birdObject = { 1000, 1000, 100, 100 };
size_t currentDepth = 0;
int main()
{
	// Generate random colliders (DEBUG)
    for (int x = 0; x < 500; x++)
	{
		int randomX = RandomGen(10, APP_SETTINGS.SCREEN_WIDTH - 74);
		int randomY = RandomGen(10, APP_SETTINGS.SCREEN_HEIGHT - 74);

		colliders.emplace_back(x, FloatRect(randomX, randomY, 64, 64));
	}

	// Create and search BVH
	BVH* bvh = new BVH();
	for (Collider& col : colliders) {
		bvh->AddCollider(&col);
	}

	bvh->Generate();

	SearchResult result = bvh->Search(birdObject);
	std::cout << "Time taken to search BVH: " << result.timeTaken << "ms" << std::endl;
	std::cout << "Amount of objects collided: " << result.collisions.size() << std::endl;
	// End of BVH create and search

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
		for (const auto& go : bvh->m_colliders) {
			window.draw(go->rectVisual);
		}
		/* BVH Visualisation */
		for (auto node : bvh->m_nodes) {
			window.draw(node->bbVisual);
			node->ChangeVisibility(currentDepth);
		}

		sf::RectangleShape birdShape;
		birdShape.setPosition(birdObject.left, birdObject.top);
		birdShape.setSize({ birdObject.width, birdObject.height });
		birdShape.setFillColor({ 255, 255, 255, 255 });
		window.draw(birdShape);

		window.display();
	}

	return 0;
}