#ifndef COLLIDER_H
#define COLLIDER_H
#include <SFML/Graphics.hpp>

class CircleCollider;
class BoxCollider;

// Collider is the base class
class Collider
{
public:
	Collider() = default;
	virtual ~Collider() = default;
public:
	// Collisions
	virtual bool CollideWith(Collider* otherCollider) const = 0;
	virtual bool CollideWith(BoxCollider* otherCollider) const = 0;
	virtual bool CollideWith(CircleCollider* otherCollider) const = 0;

	// Positions
	virtual void SetPosition(sf::Vector2f position);
	virtual void IncrementPosition(sf::Vector2f position);
	sf::Vector2f GetPosition() const;

	// Origin
	virtual void SetOrigin(sf::Vector2f origin);
	sf::Vector2f GetOrigin();

	// Collider creation
	void SetVertexCount(size_t vertexCount);
	virtual void CreateCollider() = 0;

protected:
	static bool CircleCircleCollision(const Collider* colliderA, const Collider* colliderB);
	static bool BoxCircleCollision(const BoxCollider* colliderA, const CircleCollider* colliderB);
	static bool BoxBoxCollision(const BoxCollider* colliderA, const BoxCollider* colliderB);

protected:
	sf::Vector2f m_position;
	sf::Vector2f m_origin;
	float m_angle = 0.f;
	sf::Vertex* m_vertices = nullptr;

	size_t m_vertexCount;
};

class CircleCollider : public Collider, sf::Drawable
{
	// NEED TO DO
public:
	CircleCollider() = default;
	~CircleCollider() override = default;

public:
	// Create collider
	void CreateCollider() override;

	// Collisions
	bool CollideWith(Collider* otherCollider) const override;
	bool CollideWith(BoxCollider* otherCollider) const override;
	bool CollideWith(CircleCollider* otherCollider) const override;
};

class BoxCollider : public Collider, public sf::Drawable
{
public:
	BoxCollider() = default;
	~BoxCollider() override = default;
public:
	// Collider
	void CreateCollider() override;

	// Collisions
	bool CollideWith(Collider* otherCollider) const override;
	bool CollideWith(BoxCollider* otherCollider) const override;
	bool CollideWith(CircleCollider* otherCollider) const override;

	// Size
	void SetSize(sf::Vector2f size);
	sf::Vector2f GetSize();


	// DEBUG
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::Vector2f m_size;
};
#endif