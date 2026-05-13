#include "Application.h"

#include <chrono>
#include "Random.h"
#include "Physics/SATCollision.h"

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

	// Generate random colliders (DEBUG) I hate this, everything including collision will be reworked
	for (int x = 0; x < 5; x++)
	{
		int randomX = RandomGen(10, APP_SETTINGS.SCREEN_WIDTH - 74);
		int randomY = RandomGen(10, APP_SETTINGS.SCREEN_HEIGHT - 74);
		float size = 50.0f;

        // DEBUG ONLY
        int rR = rand() % 255;
        int rG = rand() % 255;
        int rB = rand() % 255;

		// Create using random position
		sf::Vector2f newPos1 = sf::Vector2f{ static_cast<float>(randomX), static_cast<float>(randomY) } + sf::Vector2f{ 0.0f, 0.0f };
		sf::Vector2f newPos2 = sf::Vector2f{ static_cast<float>(randomX), static_cast<float>(randomY) } + sf::Vector2f{ size, 0.0f };
		sf::Vector2f newPos3 = sf::Vector2f{ static_cast<float>(randomX), static_cast<float>(randomY) } + sf::Vector2f{ size, size };
		sf::Vector2f newPos4 = sf::Vector2f{ static_cast<float>(randomX), static_cast<float>(randomY) } + sf::Vector2f{ 0.0f, size };

        // DEBUG Colour
		//newVert1.color = sf::Color(rR, rG, rB);
		//newVert2.color = sf::Color(rR, rG, rB);
		//newVert3.color = sf::Color(rR, rG, rB);
		//newVert4.color = sf::Color(rR, rG, rB);

		std::vector<sf::Vector2f> vecs = { newPos1, newPos2, newPos3, newPos4 };
		m_objects.emplace_back(vecs);
		m_objects.at(x).CreateCollider();
	}

	// Create 
	float size = 100.0f;
	sf::Vector2f newPos1 = sf::Vector2f{ 0.0f, 0.0f } + sf::Vector2f{ 0.0f, 0.0f };
	sf::Vector2f newPos2 = sf::Vector2f{ 0.0f, 0.0f } + sf::Vector2f{ size, 0.0f };
	sf::Vector2f newPos3 = sf::Vector2f{ 0.0f, 0.0f } + sf::Vector2f{ size, size };
	sf::Vector2f newPos4 = sf::Vector2f{ 0.0f, 0.0f } + sf::Vector2f{ 0.0f, size };
	std::vector<sf::Vector2f> vecs = { newPos1, newPos2, newPos3, newPos4 };
	m_testCollider = PolygonCollider(vecs);
	m_testCollider.CreateCollider();
}

void Application::Run() {
	while (m_window.isOpen()) {
		sf::Event event;
		while (m_window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				m_window.close();
		}

		m_window.clear();

		Update();

		m_window.display();
	}
}

void Application::Update() {
	float moveSpeed = 3.0f;
	float rotationSpeed = 3.0f;
	// Movement
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		m_testCollider.IncrementPosition({ 0, -moveSpeed });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		m_testCollider.IncrementPosition({ 0, moveSpeed });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		m_testCollider.IncrementPosition({ -moveSpeed, 0 });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		m_testCollider.IncrementPosition({ moveSpeed, 0 });
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
		m_testCollider.IncrementRotation(-rotationSpeed);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
		m_testCollider.IncrementRotation(rotationSpeed);
	}
	
	// draw each poly collider
	for (const auto& col : m_objects) {
	    m_window.draw(col);
	}

	// Run test collision
	for (auto& col : m_objects) {
		if (Physics::CollisionDetection::PolygonOnPolygonSATCollision(m_testCollider, col)) {
			// If collided with one object, as of now set the object to red to show collision
			m_testCollider.ChangeColour(sf::Color::Red);
			break;
		}
		m_testCollider.ChangeColour(sf::Color::White);
	}

	m_window.draw(m_testCollider);
}

void Application::Close() {
	LOG("Application Closed")
}

