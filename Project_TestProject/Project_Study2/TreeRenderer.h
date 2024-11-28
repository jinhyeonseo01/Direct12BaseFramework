#pragma once
#include "RendererComponent.h"

class TreeRenderer : public RendererComponent
{
public:
    ~TreeRenderer() override;
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
    void Rendering(const RenderPacket& renderPack) override;
    void AfterRendering() override;


    std::shared_ptr<Mesh> points;

    std::weak_ptr<GameObject> ter;

    std::weak_ptr<Texture> hmT;
    int count = 100;
};

