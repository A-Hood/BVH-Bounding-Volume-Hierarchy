#ifndef COLLIDER_H
#define COLLIDER_H
#include <SFML/Graphics.hpp>
#include "FloatRect.h"

struct Collider {
	Collider() = default;
	Collider(uint16_t _id, FloatRect _boundingBox) {
		id = _id;
		boundingBox = _boundingBox;

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
	FloatRect boundingBox;

#if _BVHDEBUG
	// DEBUG ---------------------------------------------------------------------
	sf::RectangleShape rectVisual;
#endif
};

#endif