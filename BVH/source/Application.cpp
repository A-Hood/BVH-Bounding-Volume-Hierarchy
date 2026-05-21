#include "Application.h"

#include <chrono>
#include "Random.h"

void Application::CreateApplication() {
	m_window.create(sf::VideoMode({ APP_SETTINGS.SCREEN_WIDTH, APP_SETTINGS.SCREEN_HEIGHT }), APP_SETTINGS.APPLICATION_NAME);
	m_window.setKeyRepeatEnabled(false);
	m_window.setFramerateLimit(60);

	birdObject.Initialise();
	birdObject.SetPosition({500, 400});
	birdObject.SetSize({64, 64});

	// Generate random colliders (DEBUG)
	for (size_t x = 0; x < m_numberOfObjects; x++)
	{
		int randomX = RandomGen(10, APP_SETTINGS.SCREEN_WIDTH - 74);
		int randomY = RandomGen(10, APP_SETTINGS.SCREEN_HEIGHT - 74);

		GameObject newObject;
		newObject.Initialise();
		newObject.SetPosition({static_cast<float>(randomX), static_cast<float>(randomY)});
		newObject.SetSize({64, 64});

		colliders.emplace_back(std::move(newObject));
	}

	// Great example of how using SAH is more efficient than slicing the longest node axis
	//colliders.emplace_back(sf::FloatRect(1584, 416, 64, 64));
	//colliders.emplace_back(sf::FloatRect(104, 719, 64, 64));
	//colliders.emplace_back(sf::FloatRect(412, 311, 64, 64));
	//colliders.emplace_back(sf::FloatRect(1698, 332, 64, 64));
	//colliders.emplace_back(sf::FloatRect(1808, 739, 64, 64));
	//colliders.emplace_back(sf::FloatRect(252, 179, 64, 64));
	//colliders.emplace_back(sf::FloatRect(825, 420, 64, 64));

	// Set-up BVH
	m_bvh.CreateColliderRef(colliders);
	m_bvh.GenerateBVH();

	//auto result = m_bvh.SearchBVH(birdObject);
	//LOG("Time taken to search through BVH: " + std::to_string(result.searchTime) + "ms");
	//LOG("Amount of objects collided: " + std::to_string(result.numberCollidedObjects));
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
	float moveSpeed = 10.0f;

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
	m_bvh.DrawBVH(m_window, currentDepth);
	/* BVH Visualisation */
	/* Objects Visualisation */
	for (const auto& go : colliders) {
		m_window.draw(go);
	}

	// Perform search
	//result = m_bvh.SearchBVH(birdObject);
	//std::cout << "Time taken to search BVH: " << result.searchTime << "ms" << std::endl;
	//std::cout << "Amount of objects collided: " << result.numberCollidedObjects << std::endl;

	// Set object red if collision occurs
	if (result.numberCollidedObjects > 0) {
		birdObject.GetDebugShape().setFillColor({ 255, 0, 0, 255 });
	}
	else {
		birdObject.GetDebugShape().setFillColor({ 255, 255, 255, 255 });
	}

	m_window.draw(birdObject);
#endif
}

void Application::Close() {
	LOG("Application Closed")
}
