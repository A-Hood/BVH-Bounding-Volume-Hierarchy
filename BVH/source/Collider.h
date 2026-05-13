#ifndef COLLIDER_H
#define COLLIDER_H
#include <SFML/Graphics.hpp>

// TODO:
// - Add "ColliderVisual" class to seperate visuals from implementation
// - Make rotation work properly
// - Try and find optimisations in the SAT collision
// - Major reworks across Collider + Application, implementing better and easier Collision usage
// - Origin SHOULD NOT move, it should be local to the shape

class PolygonCollider;
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
	virtual bool CollideWith(PolygonCollider* otherCollider) const = 0;
	virtual bool CollideWith(CircleCollider* otherCollider) const = 0;

	// Positions
    virtual void SetPosition(sf::Vector2f _pos) = 0;
    virtual void IncrementPosition(sf::Vector2f _pos) = 0;
	sf::Vector2f GetPosition() const;

	// Origin
    void SetOrigin(sf::Vector2f origin);
	sf::Vector2f GetOrigin();

	// Collider creation
	void SetVertexCount(size_t vertexCount);
	virtual void CreateCollider() = 0;

	// Update collider
	virtual void UpdateCollider() = 0;


protected:
	sf::Vector2f m_position;
	sf::Vector2f m_origin;
	//float m_angle = 0.f;

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
	void UpdateCollider() override {
		return;
	}

	// Collisions
	bool CollideWith(Collider* otherCollider) const override;
	bool CollideWith(PolygonCollider* otherCollider) const override;
	bool CollideWith(CircleCollider* otherCollider) const override;
};

class PolygonCollider : public Collider, public sf::Drawable
{
public:
	PolygonCollider(std::vector<sf::Vector2f>& _vertices) {
		// Move data from parameter to contained
		m_vertices = std::move(_vertices);
	}
	PolygonCollider() = default;
	~PolygonCollider() override = default;
public:
	// Create collider
	void CreateCollider() override;
	void UpdateCollider() override {
		return;
	}

	// Collisions
	bool CollideWith(Collider* otherCollider) const override;
	bool CollideWith(PolygonCollider* otherCollider) const override;
	bool CollideWith(CircleCollider* otherCollider) const override;

	const std::vector<sf::Vector2f>& GetVertices() const {
		return m_vertices;
	}

	// Need to rework
	void UpdateVertexArray() {
		for (int i = 0; i < m_vertices.size(); i++) {
			m_vertArray[i].position = m_vertices[i];
	    }
	}

	void SetPosition(sf::Vector2f _pos) override;
	void IncrementPosition(sf::Vector2f _pos) override;
	void IncrementRotation(float _rot);

	// very temp, will move all logic into a new visuals class
	void ChangeColour(const sf::Color& _colour) {
		for (int i = 0; i < m_vertArray.getVertexCount(); i++) {
			m_vertArray[i].color = _colour;
	    }
	}

	// DEBUG
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

protected:
	std::vector<sf::Vector2f> m_vertices;

private:
	// Flag for whether polygon is convex
	bool m_isConvex = true;

	sf::Vector2f m_origin = { 0.0f, 0.0f };
	float m_rotation = 0.0f;

	// Vertex array
	sf::VertexArray m_vertArrayNormal; // the origin vertex array with no position or rotation applied
	sf::VertexArray m_vertArray;
};

// PolygonCollider with defined Box parameters
class BoxCollider : public PolygonCollider {
public:
	BoxCollider(const sf::Vector2f& _pos, const sf::Vector2f& _size) : m_size(_size) {
		m_vertices.emplace_back(_pos.x, _pos.y); // Top-Left
		m_vertices.emplace_back(_pos.x + _size.x, _pos.y); // Top-Right
		m_vertices.emplace_back(_pos.x + _size.x, _pos.y + _size.y); // Bottom-Right
		m_vertices.emplace_back(_pos.x, _pos.y + _size.y); // Bottom-Left
	}

	// Creates 4 vertices, must set to a real value before usage
	BoxCollider() {
		m_vertices.emplace_back(0.0f, 0.0f);
		m_vertices.emplace_back(0.0f, 0.0f);
		m_vertices.emplace_back(0.0f, 0.0f);
		m_vertices.emplace_back(0.0f, 0.0f);
	}

	~BoxCollider() override = default;

public:
	void SetSize(sf::Vector2f _size);
    sf::Vector2f GetSize() const;

	void SetPosition(sf::Vector2f position) override;
	void IncrementPosition(sf::Vector2f position) override;

private:
	sf::Vector2f m_size;
};


#endif