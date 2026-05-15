#ifndef COLLIDER_H
#define COLLIDER_H
#include <list>
#include <SFML/Graphics.hpp>

struct Collider {
	Collider() = default;
	Collider(uint16_t _id, sf::FloatRect _boundingBox, bool _isStatic = true) {
		id = _id;
		boundingBox = _boundingBox;
		isStatic = _isStatic;

#if _BVHDEBUG
		/* SFML Specifics */
		rectVisual.setPosition(boundingBox.left, boundingBox.top);
		rectVisual.setSize({ _boundingBox.width, _boundingBox.height });

		int rR = rand() % 255;
		int rG = rand() % 255;
		int rB = rand() % 255;
		rectVisual.setFillColor(sf::Color(rR, rG, rB));
#endif
	}

	std::vector<float> GetObjectCentre()
	{
		std::vector<float> positions;
		// Centre X
		positions.push_back(boundingBox.left + (boundingBox.width / 2.f));
		// Centre Y
		positions.push_back(boundingBox.top + (boundingBox.height / 2.f));
		return positions;
	}

	sf::Vector2f GetObjectCenter() const
	{
		return {boundingBox.left + (boundingBox.width / 2.f), boundingBox.top + (boundingBox.height / 2.f)};
	}
	void SetPosition(sf::Vector2i position)
	{
		boundingBox.left = position.x;
		boundingBox.top = position.y;

#if _BVHDEBUG
		rectVisual.setPosition(boundingBox.left, boundingBox.top);
#endif
	}

	void IncrementPosition(sf::Vector2i position)
	{
		boundingBox.left += position.x;
		boundingBox.top += position.y;

#if _BVHDEBUG
		rectVisual.setPosition(boundingBox.left, boundingBox.top);
#endif
	}

	uint16_t id;
	sf::FloatRect boundingBox;
	bool isStatic = true;
	float rotation = 0.0f;

#if _BVHDEBUG
	// DEBUG ---------------------------------------------------------------------
	sf::RectangleShape rectVisual;
#endif
};

#endif