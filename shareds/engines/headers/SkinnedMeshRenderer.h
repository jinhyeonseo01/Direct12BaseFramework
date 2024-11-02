#pragma once
#include "stdafx.h"
#include "Material.h"
#include "RendererComponent.h"

class SkinnedMeshRenderer : public dxe::RendererComponent
{
public:
    SkinnedMeshRenderer();
    virtual ~SkinnedMeshRenderer() override;
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
    std::vector<std::shared_ptr<Material>> materials;
    std::vector<std::shared_ptr<Mesh>> meshList;
    std::shared_ptr<Model> model;
    std::shared_ptr<Texture> texture;
};

