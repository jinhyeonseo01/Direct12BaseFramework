#pragma once
#include "RendererComponent.h"

class SkinnedHierarchy : public dxe::RendererComponent
{
public:
    SkinnedHierarchy();
    ~SkinnedHierarchy() override;
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
