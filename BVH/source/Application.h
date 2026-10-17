#ifndef APPLICATION_H
#define APPLICATION_H
#include <cstdint>
#include <vector>

// SFML
#include <SFML/Graphics.hpp>


#include "BVHCacheLocality.h"
#include "BVHIterationOne.h"
#include "BVHIterationTwo.h"
#include "GameObject.h"

#define LOG(x) std::cout << x << std::endl;

// Simple application class for better readability, will not be included in final BVH

//class AbstractBVH;

class Application {
	struct APPLICATION_SETTINGS
	{
		const uint16_t SCREEN_WIDTH = 1920;
		const uint16_t SCREEN_HEIGHT = 1080;
		const char* APPLICATION_NAME = "BVH Visualisation";
	};
public:
	Application();
	~Application() = default;

public:
	void CreateApplication();
	void Run();
	void Update();
	void Close();
private:
	void MeasureFunctionTime(AbstractBVH& _bvhType);
private:
	// Application
	APPLICATION_SETTINGS APP_SETTINGS;
	sf::RenderWindow m_window;

	// BVH versions
	BVHIterationOne m_iterationOne;
	BVHIterationTwo m_iterationTwo;
	BVHCacheLocality m_cacheLocality;

	// Objects
	std::vector<GameObject> m_gameObjects;
	GameObject m_externalObject;
	size_t m_numberOfObjects = 1000000;

	// DEBUG ------------------------------------------------------------------------
	size_t m_currentDepth = 0;
	sf::VertexArray m_gameObjectBatch;
};

#endif