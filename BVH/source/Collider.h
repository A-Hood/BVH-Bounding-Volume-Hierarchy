#ifndef COLLIDER_H
#define COLLIDER_H
#include <SFML/Graphics.hpp>

// Currently only just a box collider,
// However in the future this should be a base class, other classes such as box and circle colliders would derive off this base class
class BoxCollider : public sf::Drawable
{
public:
	BoxCollider() = default;
	virtual ~BoxCollider() override = default;

public:
	// Positions
	void SetPosition(sf::Vector2f position);
	void IncrementPosition(sf::Vector2f position);
	sf::Vector2f GetPosition();

	// Origin
	void SetOrigin(sf::Vector2f origin);
	sf::Vector2f GetOrigin();

	// Size
	void SetSize(sf::Vector2f size);
	sf::Vector2f GetSize();

	// Collider creation
	void SetVertexCount(size_t vertexCount);
	void CreateCollider();
	sf::FloatRect GetBoundingBox() const;


	// DEBUG
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::FloatRect m_boundingBox;
	sf::Vector2f m_origin;
	sf::Vector2f* m_vertices = nullptr;

	size_t m_vertexCount;
	// DEBUG ONLY - visual purposes
	sf::RectangleShape m_bbVisual;
};
#endif