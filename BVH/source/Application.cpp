#include "Application.h"

#include <chrono>
#include "Random.h"

Application::Application() : m_bvh()
{
	birdObject.SetVertexCount(4);
}

void Application::CreateApplication() {
	m_window.create(sf::VideoMode({ APP_SETTINGS.SCREEN_WIDTH, APP_SETTINGS.SCREEN_HEIGHT }), APP_SETTINGS.APPLICATION_NAME);
	m_window.setKeyRepeatEnabled(false);
	m_window.setFramerateLimit(60);

	// Bird object
	birdObject.SetPosition({1000, 1000});
	birdObject.SetSize({64, 64});
	birdObject.CreateCollider();

	// Generate random colliders (DEBUG)
	for (int x = 0; x < 5; x++)
	{
		int randomX = RandomGen(10, APP_SETTINGS.SCREEN_WIDTH - 74);
		int randomY = RandomGen(10, APP_SETTINGS.SCREEN_HEIGHT - 74);

		m_gameObjects.emplace_back(sf::Vector2f(randomX, randomY), sf::Vector2f(64, 64), 4);
	}

	// Setup BVH
	for (GameObject& col : m_gameObjects) {
		m_bvh.AddGameObject(&col);
	}
	// Creates the bhv from the given objects
	//m_bvh.Generate();

	SearchResult result = m_bvh.Search(birdObject);
	std::cout << "Time taken to search BVH: " << result.timeTaken << "ms" << std::endl;
	std::cout << "Amount of nodes: " << result.nodes.size() << std::endl;

	/*
	auto t1 = std::chrono::high_resolution_clock::now();
	// Recalculate bounds for all collided nodes
	for (Node* node : result.nodes) {
		m_bvh.RecalculateBounds(node);
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float, std::milli> bvhRecalculate = t2 - t1;
	std::cout << "Time taken: " << bvhRecalculate.count() << "ms" << std::endl;
	*/
}

void Application::Run() {
	while (m_window.isOpen()) {
		sf::Event event;
		while (m_window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				m_window.close();

			if (event.type == sf::Event::KeyPressed)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
					currentDepth++;
					LOG(currentDepth)
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
					currentDepth--;
					LOG(currentDepth)
				}
			}
		}

		m_window.clear();

		Update();

		m_window.display();
	}
}

void Application::Update() {
	float moveSpeed = 3.0f;
	SearchResult result;
	// Movement
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		birdObject.IncrementPosition({0, -moveSpeed});
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		birdObject.IncrementPosition({0, moveSpeed});
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		birdObject.IncrementPosition({-moveSpeed, 0});
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		birdObject.IncrementPosition({moveSpeed, 0});
	}

#if _BVHDEBUG
	/* Objects Visualisation */
	for (const auto& gameObject : m_gameObjects) {
		m_window.draw(gameObject);
	}
	/* BVH Visualisation */
	for (auto node : m_bvh.GetNodes()) {
		m_window.draw(node->bbVisual);
		node->ChangeVisibility(currentDepth);
	}

	// Perform search
	result = m_bvh.Search(birdObject);
	std::cout << "Time taken to search BVH: " << result.timeTaken << "ms" << std::endl;
	std::cout << "Amount of nodes: " << result.nodes.size() << std::endl;
	//std::cout << "Amount of dynamic nodes collided: " << result.nodes.size() << std::endl;

	// Set object red if collision occurs
	/*
	if (!result.collisions.empty()) {
		birdShape.setFillColor({ 255, 0, 0, 255 });
	}
	else {
		birdShape.setFillColor({ 255, 255, 255, 255 });
	}
	birdShape.setPosition(birdObject.left, birdObject.top);
	birdShape.setSize({ birdObject.width, birdObject.height });
	*/
	m_window.draw(birdObject);
#endif
}

void Application::Close() {
	LOG("Application Closed")
}

