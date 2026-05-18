#include "Application.h"

#include <chrono>
#include "Random.h"
#include "Physics/SATCollision.h"

Application::Application() : m_bvh()
{
	//birdObject.SetVertexCount(4);
}

void Application::CreateApplication() {
	m_window.create(sf::VideoMode({ APP_SETTINGS.SCREEN_WIDTH, APP_SETTINGS.SCREEN_HEIGHT }), APP_SETTINGS.APPLICATION_NAME);
	m_window.setKeyRepeatEnabled(false);
	m_window.setFramerateLimit(60);

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

		std::vector<sf::Vector2f> vecs = { newPos1, newPos2, newPos3, newPos4 };
		m_objects.emplace_back(vecs);
		m_objects.at(x).Create();
		m_objects.at(x).ChangeColour(sf::Color(rR, rG, rB));
	}

	// Create floor (static once implemented)
	sf::Vector2f floorNewPos1 = sf::Vector2f{ 0.0f, 980.0f } + sf::Vector2f{ 0.0f, 0.0f };
	sf::Vector2f floorNewPos2 = sf::Vector2f{ 0.0f, 980.0f } + sf::Vector2f{ APPLICATION_SETTINGS().SCREEN_WIDTH, 0.0f };
	sf::Vector2f floorNewPos3 = sf::Vector2f{ 0.0f, 980.0f } + sf::Vector2f{ APPLICATION_SETTINGS().SCREEN_WIDTH, 100.0f };
	sf::Vector2f floorNewPos4 = sf::Vector2f{ 0.0f, 980.0f } + sf::Vector2f{ 0.0f, 100.0f };
	std::vector<sf::Vector2f> floorVecs = { floorNewPos1, floorNewPos2, floorNewPos3, floorNewPos4 };
	m_objects.emplace_back(floorVecs);
	m_objects.at(m_objects.size() - 1).Create();


	// Create test collider (moveable)
	float size = 100.0f;
	sf::Vector2f newPos1 = sf::Vector2f{ 0.0f, 0.0f } + sf::Vector2f{ 0.0f, 0.0f };
	sf::Vector2f newPos2 = sf::Vector2f{ 0.0f, 0.0f } + sf::Vector2f{ size, 0.0f };
	sf::Vector2f newPos3 = sf::Vector2f{ 0.0f, 0.0f } + sf::Vector2f{ size, size };
	sf::Vector2f newPos4 = sf::Vector2f{ 0.0f, 0.0f } + sf::Vector2f{ 0.0f, size };
	std::vector<sf::Vector2f> vecs = { newPos1, newPos2, newPos3, newPos4 };
	m_testCollider = PolygonCollider(vecs);
	m_testCollider.Create();
	m_testCollider.SetOrigin({ size / 2.0f, size / 2.0f });
	m_testCollider.SetPosition({ 0.0f, 0.0f });
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
	float moveSpeed = 5.0f;
	float rotationSpeed = 0.05f;
	sf::Vector2f moveDir = { 0.0f, 0.0f };

	// Speed boost
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
		moveSpeed = 10.0f;
	}

	// Movement
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		moveDir.y -= 1;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		if (!m_shouldApplyGravity) {
		    moveDir.y += 1;
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		moveDir.x -= 1;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		moveDir.x += 1;
	}
	if (abs(moveDir.x) + abs(moveDir.y) != 0.0f) {
	    // Move character by normalised direction
	    m_testCollider.IncrementPosition(Physics::CollisionDetection::Normalise(moveDir) * moveSpeed);
	}

	// Rotation
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
		m_testCollider.IncrementRotation(-rotationSpeed);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
		m_testCollider.IncrementRotation(rotationSpeed);
	}

	// Gravity
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && !m_wasGravityKeyPressed) {
		m_shouldApplyGravity = !m_shouldApplyGravity;
	}

	if (m_shouldApplyGravity) {
		// Apply gravity
		m_testCollider.IncrementPosition({ 0.0f, m_gravity });
	}

	m_wasGravityKeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::R);

	// draw each poly collider
#if SHOW_COLLIDER_VISUALS == 1
	for (auto& col : m_objects) {
		col.Update();
		col.Render(m_window);
	}
#endif

	// Run test collision
	for (auto& col : m_objects) {
		float depth = 0.0f;
		sf::Vector2f normal = { 0.0f, 0.0f };
		if (Physics::CollisionDetection::PolygonOnPolygonSATCollision(&m_testCollider, &col, depth, normal)) {
			// If collided with one object, as of now set the object to red to show collision
			m_testCollider.ChangeColour(sf::Color::Red);

			// Push collider out by depth in direction of normal, this makes all blocks act as static objects
			m_testCollider.IncrementPosition(-normal * depth);
		}
		m_testCollider.ChangeColour(sf::Color::White);
	}

#if SHOW_COLLIDER_VISUALS == 1
	m_testCollider.Update();
	m_testCollider.Render(m_window);
#endif
}

void Application::Close() {
	LOG("Application Closed")
}

