#ifndef BVH_H
#define BVH_H

#include <vector>

#include "Node.h"

struct SearchResult {
    std::vector<Collider*> collisions;
    float timeTaken;
};

class BVH {
public:
    BVH() = default;
    BVH(const int _maxDepth) {
        m_maximumDepth = _maxDepth;
    }

    ~BVH() = default;

    // Create
    void Generate();

    // Update (if dynamic)

    void AddCollider(Collider* _collider) {
        m_colliders.push_back(_collider);
    }
	std::vector<Collider*>& GetCollisionQueue() {
		return m_collisionQueue;
    }

    SearchResult Search(FloatRect _rect);
private:
	bool AABBCollision(FloatRect _boxA, FloatRect _boxB);

    FloatRect CalculateBoundingBox(const std::vector<Collider*>& nodeVector);

    bool CheckXLongestSide(FloatRect boundingBox);
	void AssignObjectSide(std::vector<Collider*>& leftSide, std::vector<Collider*>& rightSide, const Node* currentNode, float boundaryMidpoint, bool xIsLongestSide);

    void CreateNewNode(Node* currentNode, size_t currentDepth, size_t maximumDepth);

    // SEARCH ----------------------------------------------------------------------------------------------------------------------------------------------------------
    void RecursiveSearch(FloatRect searchRect, Node* currentNode);
public:
    std::vector<Node*> m_nodes;
    std::vector<Collider*> m_colliders;
private:

	std::vector<Collider*> m_collisionQueue;

    int m_maximumDepth;
};

#endif
