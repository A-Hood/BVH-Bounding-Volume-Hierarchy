#ifndef BVH_H
#define BVH_H

#include <vector>

#include "GameObject.h"
#include "Node.h"

struct SearchResult {
    ~SearchResult() {
        nodes.clear();
    }

    std::vector<Node*> nodes;
    float timeTaken;
};

class BVH {
public:
    BVH() = default;
    BVH(const int _maxDepth) {
        m_maximumDepth = _maxDepth;
    }
    ~BVH() = default;

public:
    // Update (if dynamic)
    void AddGameObject(GameObject* collider);
	//std::vector<GameObject*>& GetCollisionQueue();
    // Create
    void Generate();
    // Nodes
    std::vector<Node*>& GetNodes();

    // Search functions
    SearchResult Search(const GameObject& targetObject);

    void RecalculateBounds(Node* currentNode);

private:
    sf::FloatRect CalculateBoundingBox(const std::vector<GameObject*>& nodeVector);

    bool CheckXLongestSide(const sf::FloatRect& boundingBox);
	void AssignObjectSide(std::vector<GameObject*>& leftSide, std::vector<GameObject*>& rightSide, const Node* currentNode, float boundaryMidpoint, bool xIsLongestSide);

    void CreateNewNode(Node* currentNode, size_t currentDepth, size_t maximumDepth);

    // SEARCH ----------------------------------------------------------------------------------------------------------------------------------------------------------
    void RecursiveSearch(const GameObject& targetObject, Node* currentNode);

private:
    // Nodes containing searched object
	std::vector<Node*> m_nodesQueue;

    std::vector<GameObject*> m_ptrGameObjects;
    std::vector<Node*> m_nodes;
    int m_maximumDepth;
};

#endif
