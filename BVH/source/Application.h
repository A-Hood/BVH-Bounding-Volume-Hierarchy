#ifndef APPLICATION_H
#define APPLICATION_H
#include <cstdint>
#include <vector>
#include <iostream>

// SFML
#include <SFML/Graphics.hpp>

#include "BoundingVolumeHierarchy.h"
#include "Collider.h"
#include "FloatRect.h"

#define LOG(x) std::cout << x << std::endl;

// Simple application class for better readability, will not be included in final BVH

class Application {
	struct APPLICATION_SETTINGS {
		const uint16_t SCREEN_WIDTH = 1920;
		const uint16_t SCREEN_HEIGHT = 1080;
		const char* APPLICATION_NAME = "BVH Visualisation";
	};

	~Application() = default;
public:
	void CreateApplication();
	void Run();
	void Update();
	void Close();

private:
	APPLICATION_SETTINGS APP_SETTINGS = APPLICATION_SETTINGS();

	sf::RenderWindow m_window;

	BVH m_bvh = BVH();

	// DEBUG ------------------------------------------------------------------------
	std::vector<Collider> colliders;
	FloatRect birdObject = { 1000, 1000, 100, 100 };
	sf::RectangleShape birdShape;
	size_t currentDepth = 0;
	size_t m_numberOfObjects = 500;
};

#endif