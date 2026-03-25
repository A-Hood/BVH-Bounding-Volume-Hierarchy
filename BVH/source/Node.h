#ifndef NODE_H
#define NODE_H

#include "Collider.h"
#include "GameObject.h"

class Node : public BoxCollider {
public:
	Node() = default;
	~Node() = default;

	// Defines GameObjects within that node
	void DefineObjects(std::vector<GameObject*> _colliders)
	{
		m_colliders = _colliders;
	}
	// Bounds of the Node
	void DefineBounds(const sf::FloatRect& _boundingBox)
	{
		SetPosition({_boundingBox.left, _boundingBox.top});
		SetSize({_boundingBox.width, _boundingBox.height});

#if _BVHDEBUG
		/* SFML Stuff */
		bbVisual.setPosition(GetPosition());
		bbVisual.setSize(GetSize());
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

	std::vector<GameObject*> m_colliders;

	size_t depth = 1;
};

#endif