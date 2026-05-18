#ifndef COLLIDER_H
#define COLLIDER_H

// Collider Visualisation
#include "ColliderVisual.h"

// SFML
#include <SFML/Graphics.hpp>

#ifdef _DEBUG
#define SHOW_COLLIDER_VISUALS 1
#else
#define SHOW_COLLIDER_VISUALS 0
#endif

class PolygonCollider;
class CircleCollider;

// Base class collider, only stores pure virtual functions as each collider type has personalised data to store
class Collider
{
public:
	virtual ~Collider() = default;

public:
	// Creation
	virtual void Create() = 0;

	// Collision
	virtual bool CollideWith(Collider* _other) const = 0;
	virtual bool CollideWith(PolygonCollider* _other) const = 0;
	virtual bool CollideWith(CircleCollider* _other) const = 0;

	// Position
    virtual void SetPosition(sf::Vector2f _pos) = 0;
    virtual void IncrementPosition(sf::Vector2f _pos) = 0;
	virtual sf::Vector2f GetPosition() const = 0; // Must override with valid position

	// Rotation
	virtual void SetRotation(float _rot) = 0;
	virtual void IncrementRotation(float _rot) = 0;
	virtual float GetRotation() const = 0; // Must override with valid rotation
};

#if SHOW_COLLIDER_VISUALS == 1
class CircleCollider : public Collider, public CircleColliderVisual
#else
class CircleCollider : public Collider
#endif
{
public:
	CircleCollider() = default;
	CircleCollider(float _rad, const sf::Vector2f& _pos);
	~CircleCollider() override = default;

public:
	// Create collider
	void Create() override;

	// Update from the ColliderVisual base class
#if SHOW_COLLIDER_VISUALS == 1
	void Update() override {
		UpdateCircleShape(m_radius, m_centre);
	}

	void Render(sf::RenderWindow& _window) const override {
		CircleColliderVisual::Render(_window);
	}
#endif

	// Collisions
	bool CollideWith(Collider* otherCollider) const override { return false; }
	bool CollideWith(PolygonCollider* otherCollider) const override { return false; }
	bool CollideWith(CircleCollider* otherCollider) const override { return false; }

	// Position
    void SetPosition(sf::Vector2f _pos) override;
    void IncrementPosition(sf::Vector2f _pos) override;
    sf::Vector2f GetPosition() const override;

	// Rotation
    void SetRotation(float _rot) override;
    void IncrementRotation(float _rot) override;
    float GetRotation() const override;

	// Radius
	void SetRadius(float _rad);
	float GetRadius() const;

private:
	sf::Vector2f m_centre = { 0.0f, 0.0f };
	float m_radius = 0.0f;
};

#if SHOW_COLLIDER_VISUALS == 1
class PolygonCollider : public Collider, public PolygonColliderVisual
#else
class PolygonCollider : public Collider
#endif
{
public:
	PolygonCollider() = default;
	PolygonCollider(std::vector<sf::Vector2f>& _vertices) {
		// Move data from parameter to contained
		m_verticesLocal = std::move(_vertices);
		m_verticesGlobal = m_verticesLocal;
	}
	~PolygonCollider() override = default;
public:
	// Create collider
	void Create() override;

	// Update from the ColliderVisual base class
#if SHOW_COLLIDER_VISUALS == 1
	void Update() override {
		UpdatePolygonShape(m_verticesGlobal); // Update this with params when completed
	}

	void Render(sf::RenderWindow& _window) const override {
		PolygonColliderVisual::Render(_window);
	}
#endif

	// Collisions
	bool CollideWith(Collider* otherCollider) const override { return false; }
	bool CollideWith(PolygonCollider* otherCollider) const override { return false; }
	bool CollideWith(CircleCollider* otherCollider) const override { return false; }

	// Position
	void SetPosition(sf::Vector2f _pos) override;
	void IncrementPosition(sf::Vector2f _pos) override;
	sf::Vector2f GetPosition() const override;

	// Rotation
	void SetRotation(float _rot) override;
	void IncrementRotation(float _rot) override;
	float GetRotation() const override;

	// Origin
	void SetOrigin(const sf::Vector2f& _localPos);
	sf::Vector2f GetLocalOrigin() const;
	sf::Vector2f GetGlobalOrigin() const;

	sf::Vector2f GetCentre() const;

	// Vertices
	const std::vector<sf::Vector2f>& GetVertices() const {
		return m_verticesGlobal;
	}

protected:
	std::vector<sf::Vector2f> m_verticesLocal;
	std::vector<sf::Vector2f> m_verticesGlobal;

	sf::Vector2f m_originLocal = { 0.0f, 0.0f };
	sf::Vector2f m_originGlobal = { 0.0f, 0.0f };

    float m_rotation = 0.0f;
};

// PolygonCollider with defined Box parameters
class BoxCollider : public PolygonCollider {
public:
	BoxCollider();
	BoxCollider(const sf::Vector2f& _pos, const sf::Vector2f& _size);

	~BoxCollider() override = default;

public:
	void SetSize(sf::Vector2f _size);
    sf::Vector2f GetSize() const;

private:
	sf::Vector2f m_size;
};

#endif