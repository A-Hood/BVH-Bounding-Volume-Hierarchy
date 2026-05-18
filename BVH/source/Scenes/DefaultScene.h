#ifndef DEFAULTSCENE_H
#define DEFAULTSCENE_H

#include "Scene.h"

class DefaultScene : public Scene {
public:
    DefaultScene(const uint16_t& _width, const uint16_t& _height) : Scene("DefaultScene"), m_windowWidth(_width), m_windowHeight(_height) {}
    ~DefaultScene() override = default;

public:
    void Initialise() override;
    void Update() override;

    void CollisionUpdate() override;

    void Close() override;

private:
    uint16_t m_windowWidth;
    uint16_t m_windowHeight;

    PolygonCollider* m_mainCollider = nullptr;
    PolygonCollider* m_floorCollider = nullptr;


    bool m_shouldApplyGravity = false;
    bool m_wasGravityKeyPressed = false;
    float m_gravity = 9.0f;
};

#endif