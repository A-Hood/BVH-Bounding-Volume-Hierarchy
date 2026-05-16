#ifndef COLLIDERVISUAL_H
#define COLLIDERVISUAL_H

#include <memory>

// SFML
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shape.hpp>

class ColliderVisual {
public:
	virtual ~ColliderVisual() = default;
public:
	virtual void Render(sf::RenderWindow& _window) const = 0;
	virtual void ChangeColour(const sf::Color& _colour) = 0;

	virtual void Update() = 0;
};

class CircleColliderVisual : public ColliderVisual {
public:
    ~CircleColliderVisual() override = default;
public:
	void Render(sf::RenderWindow& _window) const override {
		_window.draw(*m_outline);
	}
	void ChangeColour(const sf::Color& _colour) override {
		m_outline->setOutlineColor(_colour);
	}

	void UpdateCircleShape(float _rad, const sf::Vector2f& _pos) const {
		m_outline->setOrigin(_pos);
		m_outline->setScale(_rad * 2.0f, _rad * 2.0f);
	}
protected:
	std::shared_ptr<sf::Shape> m_outline;
};

// Represents global vertices of the polygon collider
class PolygonColliderVisual : public ColliderVisual {
public:
    ~PolygonColliderVisual() override = default;
public:
	void Render(sf::RenderWindow& _window) const override {
		_window.draw(m_vertices);
	}
	void ChangeColour(const sf::Color& _colour) override {
		for (int i = 0; i < static_cast<int>(m_vertices.getVertexCount()); i++) {
			m_vertices[i].color = _colour;
		}
	}

	void CreateVertexArray(const std::vector<sf::Vector2f>& _vertPosVec) {
		m_vertices.clear();
		m_vertices.resize(_vertPosVec.size());
		m_vertices.setPrimitiveType(sf::Quads);

	}
	void UpdatePolygonShape(const std::vector<sf::Vector2f>& _vertPosVec) {
	    for (int i = 0; i < _vertPosVec.size(); i++) {
			m_vertices[i].position = _vertPosVec[i];
	    }
	}
private:
	sf::VertexArray m_vertices;
};

#endif