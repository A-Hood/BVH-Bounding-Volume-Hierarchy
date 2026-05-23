#ifndef COLLIDER_H
#define COLLIDER_H
#include <SFML/Graphics.hpp>
#include "FloatRect.h"

struct Collider {
	Collider() = default;
	Collider(uint16_t _id, FloatRect _boundingBox, bool _isStatic = true) {
		id = _id;
		boundingBox = _boundingBox;
		isStatic = _isStatic;

		/* SFML Specifics */
		rectVisual.setPosition(boundingBox.left, boundingBox.top);
		rectVisual.setSize({ _boundingBox.width, _boundingBox.height });

		int rR = rand() % 255;
		int rG = rand() % 255;
		int rB = rand() % 255;
		rectVisual.setFillColor(sf::Color(rR, rG, rB));
	}

	void SetPosition(sf::Vector2i position)
	{
		boundingBox.left = position.x;
		boundingBox.top = position.y;

		rectVisual.setPosition(boundingBox.left, boundingBox.top);
	}

	void IncrementPosition(sf::Vector2i position)
	{
		boundingBox.left += position.x;
		boundingBox.top += position.y;

		rectVisual.setPosition(boundingBox.left, boundingBox.top);
	}

	uint16_t id;
	FloatRect boundingBox;
	bool isStatic = true;
	float rotation = 0.0f;

	// DEBUG ---------------------------------------------------------------------
	sf::RectangleShape rectVisual;
};

#endif