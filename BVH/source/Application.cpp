#include "Application.h"
#include "Application.h"
#include "Application.h"

#include <chrono>
#include "Random.h"
#include "Physics/SATCollision.h"

Application::Application() : m_bvh(), m_scene(APPLICATION_SETTINGS().SCREEN_WIDTH, APPLICATION_SETTINGS().SCREEN_HEIGHT)
{
	//birdObject.SetVertexCount(4);
}

void Application::CreateApplication() {
	m_window.create(sf::VideoMode({ APP_SETTINGS.SCREEN_WIDTH, APP_SETTINGS.SCREEN_HEIGHT }), APP_SETTINGS.APPLICATION_NAME);
	m_window.setKeyRepeatEnabled(false);
	m_window.setFramerateLimit(60);

	m_scene.Initialise();
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

		Render();

		m_window.display();
	}
}

void Application::Update() {
	// Update scene state
	m_scene.Update();

	// Update collisions and apply resolution
	m_scene.CollisionUpdate();

	// Apply updates to colliders (mainly visual)
    m_scene.ColliderUpdate();
}

void Application::Render() {
	m_scene.Render(m_window);
}

void Application::Close() {
	m_scene.Close();

	LOG("Application Closed")
}