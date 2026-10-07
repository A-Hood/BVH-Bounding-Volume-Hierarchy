#ifndef APPLICATION_H
#define APPLICATION_H
#include <cstdint>
#include <vector>
#include <iostream>

// SFML
#include <SFML/Graphics.hpp>

#include "BoundingVolumeHierarchy.h"
#include "GameObject.h"

#define LOG(x) std::cout << x << std::endl;

// Simple application class for better readability, will not be included in final BVH

class Application {
	struct APPLICATION_SETTINGS
	{
		const uint16_t SCREEN_WIDTH = 1920;
		const uint16_t SCREEN_HEIGHT = 1080;
		const char* APPLICATION_NAME = "BVH Visualisation";
	};
public:
	Application() : m_bvh() {}
	~Application() = default;

public:
	void CreateApplication();
	void Run();
	void Update();
	void Close();

private:
	// Application
	APPLICATION_SETTINGS APP_SETTINGS;
	sf::RenderWindow m_window;

	// BVH
	BVH m_bvh;

	// Objects
	std::vector<GameObject> m_gameObjects;
	GameObject m_externalObject;
	size_t m_numberOfObjects = 8;

	// DEBUG ------------------------------------------------------------------------
	size_t m_currentDepth = 0;
	sf::VertexArray m_gameObjectBatch;
};

#endif