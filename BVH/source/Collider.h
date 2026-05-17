#ifndef COLLIDER_H
#define COLLIDER_H
#include <cassert>
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

	int GetObjectCentreFromAxis(const int axis) const
	{
		// 0 -> X
		// 1 -> Y
		switch (axis)
		{
		case 0:
			return static_cast<int>(boundingBox.left) + static_cast<int>(boundingBox.width / 2.f);
		case 1:
			return static_cast<int>(boundingBox.top) + static_cast<int>(boundingBox.height / 2.f);
		default:
			assert(axis < 1, "Invalid axis");
			return 0;
		}
	}

	std::vector<float> GetObjectCentre() const
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