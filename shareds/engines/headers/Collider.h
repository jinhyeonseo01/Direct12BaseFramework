#pragma once
#include <stdafx.h>

#include "Component.h"

enum class ColliderType
{
    Box,
    Sphere,
    Capsule,
    Mesh,
};

class Collider : public Component
{
private:
    BoundingBox aabbBox;
public:
    ColliderType type = ColliderType::Box;

    BoundingSphere sphere;

    bool _convex = false;

    BoundingBox GetAABBBound();

public:
    ~Collider() override;
    void* Clone() override;
    void ReRef() override;
    void Destroy() override;
    void Init() override;
    void Start() override;
    void Update() override;
    void LateUpdate() override;
    void OnEnable() override;
    void OnDisable() override;
    void OnDestroy() override;
    void OnComponentDestroy() override;
    void BeforeRendering() override;
    void Rendering() override;
    void AfterRendering() override;
};

