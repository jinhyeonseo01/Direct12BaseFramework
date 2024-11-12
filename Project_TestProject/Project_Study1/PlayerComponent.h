#pragma once
#include "stdafx.h"
#include "Component.h"
#include "MeshRenderer.h"
#include "Texture.h"

class PlayerComponent : public Component
{
public:
    ~PlayerComponent() override;
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

    std::weak_ptr<GameObject> topRoter;
    std::vector<std::weak_ptr<GameObject>> backRoter;
    std::weak_ptr<GameObject> camera;
    std::weak_ptr<GameObject> ter;

    std::weak_ptr<GameObject> menu;
    std::weak_ptr<GameObject> menu_start;
    std::weak_ptr<GameObject> menu_help;
    std::weak_ptr<GameObject> menu_info;

    std::weak_ptr<Texture> hmT;

    std::vector<std::weak_ptr<MeshRenderer>> boxs;

    static bool isMenu;
    static bool isMenuHelp;

    Vector3 velocity = Vector3::Zero;
    Vector2 prevPos = Vector2::Zero;
    bool cameraControl = false;
    Vector3 angle = Vector3::Zero;

    bool isPlayer = true;

    float time = 5;
};

