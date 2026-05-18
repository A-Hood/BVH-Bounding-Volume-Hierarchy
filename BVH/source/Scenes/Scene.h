#ifndef SCENE_H
#define SCENE_H
#include <memory>
#include <vector>

#include "../Physics/Colliders/Collider.h"
#include "../Physics/SATCollision.h"

// Comments tend to be a note-to-self as I forget and am stupid, helps to note down stuff

class Scene {
public:
    Scene(const char* _name) : m_sceneName(_name) {}
    virtual ~Scene() = default;
    
public:
    virtual void Initialise() = 0;

    virtual void Update() = 0;

    void ColliderUpdate() const {
        for (auto& col : m_colliders) {
            col->Update();
        } 
    }
    
    // Base Collision checks every collider against every other collider, can override for unique collisions
    virtual void CollisionUpdate() {
        for (auto& col1 : m_colliders) {
            for (auto& col2 : m_colliders) {
                // If collider is self
                if (col1 == col2) {
                    continue;
                }

                float depth = 0.0f;
                sf::Vector2f normal = { 0.0f, 0.0f };

                // Only have polygon collision set-up whoops, will add circle collision soon
                if (Physics::CollisionDetection::PolygonOnPolygonSATCollision(col1.get(), col2.get(), depth, normal)) {
                    col1.get()->IncrementPosition(-normal * depth / 2.0f);
                    col2.get()->IncrementPosition(normal * depth / 2.0f);
                }
            }

        }
    }

    // Base class render performs collider visuals rendering
    virtual void Render(sf::RenderWindow& _window) {
#if SHOW_COLLIDER_VISUALS == 1
        for (auto& col : m_colliders) {
            col->Render(_window);
        }
#endif
    }

    virtual void Close() = 0;

protected:
    void AddCollider(std::unique_ptr<PolygonCollider> _col) {
        m_colliders.push_back(std::move(_col));
    }
    // Get end
    PolygonCollider* GetEnd() const {
        return m_colliders.at(m_colliders.size() - 1).get();
    }
    // Remove Collider needs implementing

    const char* GetName() const {
        return m_sceneName;
    }

protected:
    // Would be much better not to create these dynamically, instead with a fixed object pool
    // This will also need to be switched to base Collider type, do not have any circle collision
    // so useless right now
    std::vector<std::unique_ptr<PolygonCollider>> m_colliders;

private:
    const char* m_sceneName = nullptr;
#if SHOW_COLLIDER_VISUALS == 1
    //std::vector<std::unique_ptr<ColliderVisual>> m_colliderVisuals;
#endif

    // Would also store GameObjects + more required for scene updating, seperating scene entirely from
    // the application
    // A Collision/Physics World may be a good idea to be responsible solely for physics simulations,
    // keeping it seperate from regular game logic, will think about it
};

#endif