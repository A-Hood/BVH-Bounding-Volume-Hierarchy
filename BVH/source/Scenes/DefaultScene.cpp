#include "DefaultScene.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>


#include "../Application.h"
#include "../Physics/SATCollision.h"

int RandomGen(size_t minValue, size_t maxValue);

void DefaultScene::Initialise() {
	// Generate random colliders (DEBUG) I hate this, everything including collision will be reworked
	for (int x = 0; x < 100; x++)
	{
		int randomX = RandomGen(10, m_windowWidth - 74);
		int randomY = RandomGen(10, m_windowHeight - 74);
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
		AddCollider(std::make_unique<PolygonCollider>(vecs));
		GetEnd()->Create();
		GetEnd()->ChangeColour(sf::Color(rR, rG, rB));
	}

	// Create floor (static once implemented)
	sf::Vector2f floorNewPos1 = sf::Vector2f{ 0.0f, 980.0f } + sf::Vector2f{ 0.0f, 0.0f };
	sf::Vector2f floorNewPos2 = sf::Vector2f{ 0.0f, 980.0f } + sf::Vector2f{ static_cast<float>(m_windowWidth), 0.0f };
	sf::Vector2f floorNewPos3 = sf::Vector2f{ 0.0f, 980.0f } + sf::Vector2f{ static_cast<float>(m_windowWidth), 100.0f };
	sf::Vector2f floorNewPos4 = sf::Vector2f{ 0.0f, 980.0f } + sf::Vector2f{ 0.0f, 100.0f };
	std::vector<sf::Vector2f> floorVecs = { floorNewPos1, floorNewPos2, floorNewPos3, floorNewPos4 };
	AddCollider(std::make_unique<PolygonCollider>(floorVecs));
	GetEnd()->Create();


	// Create test collider (moveable)
	float size = 100.0f;
	sf::Vector2f newPos1 = sf::Vector2f{ 0.0f, 0.0f } + sf::Vector2f{ 0.0f, 0.0f };
	sf::Vector2f newPos2 = sf::Vector2f{ 0.0f, 0.0f } + sf::Vector2f{ size, 0.0f };
	sf::Vector2f newPos3 = sf::Vector2f{ 0.0f, 0.0f } + sf::Vector2f{ size, size };
	sf::Vector2f newPos4 = sf::Vector2f{ 0.0f, 0.0f } + sf::Vector2f{ 0.0f, size };
	std::vector<sf::Vector2f> vecs = { newPos1, newPos2, newPos3, newPos4 };
	AddCollider(std::make_unique<PolygonCollider>(vecs));
	m_mainCollider = GetEnd();
	GetEnd()->Create();
	GetEnd()->SetOrigin({ size / 2.0f, size / 2.0f });
	GetEnd()->SetPosition({ 0.0f, 0.0f });

	LOG("Created " + std::string(GetName()))
}

void DefaultScene::Update() {
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
		m_mainCollider->IncrementPosition(Physics::CollisionDetection::Normalise(moveDir) * moveSpeed);
	}

	// Rotation
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
		m_mainCollider->IncrementRotation(-rotationSpeed);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
		m_mainCollider->IncrementRotation(rotationSpeed);
	}

	// Gravity
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && !m_wasGravityKeyPressed) {
		m_shouldApplyGravity = !m_shouldApplyGravity;
	}

	if (m_shouldApplyGravity) {
		// Apply gravity
		m_mainCollider->IncrementPosition({ 0.0f, m_gravity });
	}

	m_wasGravityKeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::R);

}

void DefaultScene::Close() {
	LOG("Closed " + std::string(GetName()))
}
