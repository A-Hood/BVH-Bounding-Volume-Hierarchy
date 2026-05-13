#include "Application.h"

#include <chrono>
#include "Random.h"

void Application::CreateApplication() {
	m_window.create(sf::VideoMode({ APP_SETTINGS.SCREEN_WIDTH, APP_SETTINGS.SCREEN_HEIGHT }), APP_SETTINGS.APPLICATION_NAME);
	m_window.setKeyRepeatEnabled(false);
	m_window.setFramerateLimit(60);

	// Generate random colliders (DEBUG)
	for (size_t x = 0; x < m_numberOfObjects; x++)
	{
		int randomX = RandomGen(10, APP_SETTINGS.SCREEN_WIDTH - 74);
		int randomY = RandomGen(10, APP_SETTINGS.SCREEN_HEIGHT - 74);

		colliders.emplace_back(x, FloatRect(randomX, randomY, 64, 64), false);
	}

	// Set-up BVH
	for (Collider& col : colliders) {
		m_bvh.AddCollider(&col);
	}
	m_bvh.Generate();
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
		birdObject.top -= moveSpeed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		birdObject.top += moveSpeed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		birdObject.left -= moveSpeed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		birdObject.left += moveSpeed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
		birdObject.left -= moveSpeed;
	}

#if _BVHDEBUG
	/* Objects Visualisation */
	for (const auto& go : colliders) {
		m_window.draw(go.rectVisual);
	}
	/* BVH Visualisation */
	m_bvh.DrawBVH(m_window, m_bvh.GetMasterNode(), currentDepth);

	// Perform search
	result = m_bvh.SearchBVH(birdObject);
	std::cout << "Time taken to search BVH: " << result.searchTime << "ms" << std::endl;
	std::cout << "Amount of objects collided: " << result.numberCollidedObjects << std::endl;

	// Set object red if collision occurs
	if (result.numberCollidedObjects > 0) {
		birdShape.setFillColor({ 255, 0, 0, 255 });
	}
	else {
		birdShape.setFillColor({ 255, 255, 255, 255 });
	}
	birdShape.setPosition(birdObject.left, birdObject.top);
	birdShape.setSize({ birdObject.width, birdObject.height });

	m_window.draw(birdShape);
#endif
}

void Application::Close() {
	LOG("Application Closed")
}
