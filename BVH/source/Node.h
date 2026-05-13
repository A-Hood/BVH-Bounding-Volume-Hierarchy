#ifndef NODE_H
#define NODE_H

#include "Collider.h"

class Node {
public:
	Node() = default;
	~Node() = default;

	// Defines GameObjects within that node
	void DefineColliders(std::vector<Collider*> _colliders)
	{
		m_colliders = _colliders;
	}
	std::vector<Collider*>& GetCollders()
	{
		return m_colliders;
	}
	// Bounds of the Node
	void DefineBounds(const FloatRect& _boundingBox)
	{
		boundingBox.left = _boundingBox.left;
		boundingBox.top = _boundingBox.top;
		boundingBox.width = _boundingBox.width;
		boundingBox.height = _boundingBox.height;

#if _BVHDEBUG
		/* SFML Stuff */
		bbVisual.setPosition(boundingBox.left, boundingBox.top);
		bbVisual.setSize({ boundingBox.width, boundingBox.height });
		bbVisual.setOutlineColor(sf::Color::Red);
		bbVisual.setOutlineThickness(3);
		bbVisual.setFillColor(sf::Color(0, 0, 0, 0));
#endif
	}
	// Define the previous nodes and child nodes
	void DefineChildA(Node* _childA)
	{
		this->childA = _childA;
	}

	void DefineChildB(Node* _childB)
	{
		this->childB = _childB;
	}

	void DefineParentNode(Node* _parentNode)
	{
		this->previousNode = _parentNode;
	}

	void DefineDepth(size_t _depth)
	{
		this->depth = _depth;
	}

#if _BVHDEBUG
	void ChangeVisibility(size_t currentDepth)
	{
		if (currentDepth == depth || currentDepth == 0)
		{
			bbVisual.setOutlineColor(sf::Color::Red);
		}
		else
		{
			bbVisual.setOutlineColor(sf::Color::Transparent);
		}
	}
	sf::RectangleShape bbVisual;
#endif

public:
	Node* previousNode = nullptr;
	Node* childA = nullptr;
	Node* childB = nullptr;

	std::vector<Collider*> m_colliders;
	FloatRect boundingBox;

	size_t depth = 1;
};

#endif