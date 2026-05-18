#ifndef APPLICATION_H
#define APPLICATION_H
#include <cstdint>
#include <vector>
#include <iostream>

// SFML
#include <SFML/Graphics.hpp>

#include "BoundingVolumeHierarchy.h"
#include "GameObject.h"
#include "Scenes/DefaultScene.h"

#define LOG(x) std::cout << x << std::endl;

// Simple application class for better readability, will not be included in final BVH

class Application {
public:
	Application();
	~Application() = default;

	void CreateApplication();
	void Run();
	void Update();
	void Render();
	void Close();
private:
	struct APPLICATION_SETTINGS {
		const uint16_t SCREEN_WIDTH = 1920;
		const uint16_t SCREEN_HEIGHT = 1080;
		const char* APPLICATION_NAME = "BVH Visualisation";
	};
	// Application window
	APPLICATION_SETTINGS APP_SETTINGS = APPLICATION_SETTINGS();
	sf::RenderWindow m_window;

	// BVH
	BVH m_bvh;
	size_t currentDepth = 0;

	// Game objects
	std::vector<GameObject> m_gameObjects;

	// DEBUG ------------------------------------------------------------------------
	GameObject birdObject;

	PolygonCollider m_testCollider;
	std::vector<PolygonCollider> m_objects;

	DefaultScene m_scene;
};

#endif