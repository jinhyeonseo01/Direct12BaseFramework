#pragma once
#include <stdafx.h>

#include "Camera.h"
#include "Component.h"

class CameraController : public Component
{
public:
    ~CameraController() override;
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


public:
    std::shared_ptr<Camera> camera;


    Vector2 prevMourePosition;
    Vector3 totalAngle = Vector3::Zero;
    bool cameraControl = false;
};

