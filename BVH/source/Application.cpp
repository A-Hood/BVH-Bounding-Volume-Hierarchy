#include "Application.h"

#include <chrono>
#include "Random.h"

void Application::CreateApplication() {
	m_window.create(sf::VideoMode({ APP_SETTINGS.SCREEN_WIDTH, APP_SETTINGS.SCREEN_HEIGHT }), APP_SETTINGS.APPLICATION_NAME);
	m_window.setKeyRepeatEnabled(false);
	m_window.setFramerateLimit(60);

	m_gameObjectBatch.setPrimitiveType(sf::Quads);
	m_gameObjectBatch.resize(m_numberOfObjects * 4);

	// Generate random colliders (DEBUG)
	for (size_t x = 0; x < m_numberOfObjects; x++)
	{
		int randomX = RandomGen(10, APP_SETTINGS.SCREEN_WIDTH - 74);
		int randomY = RandomGen(10, APP_SETTINGS.SCREEN_HEIGHT - 74);

		GameObject newObject;
		newObject.Initialise();
		newObject.SetPosition({static_cast<float>(randomX), static_cast<float>(randomY)});
		newObject.SetSize({64, 64});

		auto& vertices = newObject.GetVertexArray();
		m_gameObjectBatch[(x * 4) + 0].position = vertices[0].position;
		m_gameObjectBatch[(x * 4) + 1].position = vertices[1].position;
		m_gameObjectBatch[(x * 4) + 2].position = vertices[2].position;
		m_gameObjectBatch[(x * 4) + 3].position = vertices[3].position;

		m_gameObjectBatch[(x * 4) + 0].color = vertices[0].color;
		m_gameObjectBatch[(x * 4) + 1].color = vertices[1].color;
		m_gameObjectBatch[(x * 4) + 2].color = vertices[2].color;
		m_gameObjectBatch[(x * 4) + 3].color = vertices[3].color;

		// Add object to vertex batch
		m_gameObjects.emplace_back(std::move(newObject));
	}

	// Great example of how using SAH is more efficient than slicing the longest node axis
	//colliders.emplace_back(0, FloatRect(1584, 416, 64, 64));
	//colliders.emplace_back(1, FloatRect(104, 719, 64, 64));
	//colliders.emplace_back(2, FloatRect(412, 311, 64, 64));
	//colliders.emplace_back(3, FloatRect(1698, 332, 64, 64));
	//colliders.emplace_back(4, FloatRect(1808, 739, 64, 64));
	//colliders.emplace_back(5, FloatRect(252, 179, 64, 64));
	//colliders.emplace_back(6, FloatRect(825, 420, 64, 64));

	// Set-up BVH
	for (auto& collider : m_gameObjects)
	{
		m_bvh.AddCollider(&collider.GetCollider());
	}
	m_bvh.Generate();
}

void Application::Run()
{
	while (m_window.isOpen()) {
		sf::Event event;
		while (m_window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				m_window.close();

			if (event.type == sf::Event::KeyPressed)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
					m_currentDepth++;
					LOG(m_currentDepth)
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
					m_currentDepth--;
					LOG(m_currentDepth)
				}
			}
		}

		m_window.clear();

		Update();

		m_window.display();
	}
}

void Application::Update()
{
	float moveSpeed = 3.0f;
	SearchResult result;
	// Movement
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
	//	birdObject.top -= moveSpeed;
	//}
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
	//	birdObject.top += moveSpeed;
	//}
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
	//	birdObject.left -= moveSpeed;
	//}
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
	//	birdObject.left += moveSpeed;
	//}
	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
	//	birdObject.left -= moveSpeed;
	//}

	/* Objects Visualisation */
	//for (const auto& go : colliders) {
	//	m_window.draw(go.rectVisual);
	//}
	m_window.draw(m_gameObjectBatch);
	/* BVH Visualisation */
	m_bvh.Draw(m_window, m_bvh.GetMasterNode(), m_currentDepth);

	// Perform search
	//result = m_bvh.SearchBVH(birdObject);
	////std::cout << "Time taken to search BVH: " << result.searchTime << "ms" << std::endl;
	////std::cout << "Amount of objects collided: " << result.numberCollidedObjects << std::endl;
	//
	//// Set object red if collision occurs
	//if (result.numberCollidedObjects > 0) {
	//	birdShape.setFillColor({ 255, 0, 0, 255 });
	//}
	//else {
	//	birdShape.setFillColor({ 255, 255, 255, 255 });
	//}
	//birdShape.setPosition(birdObject.left, birdObject.top);
	//birdShape.setSize({ birdObject.width, birdObject.height });

	//m_window.draw(birdShape);
}

void Application::Close() {
	LOG("Application Closed")
}
