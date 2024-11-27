#pragma once
#include "Component.h"

class Bullet : public Component
{
public:
    ~Bullet() override;
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

    float time = 10;
};
