#pragma once
#include "Material.h"
#include "MeshRenderer.h"
#include "Scene.h"

class Study1Scene : public Scene
{
public:
    void* Clone() override;
    void ReRef() override;
    Study1Scene();
    Study1Scene(std::wstring name);
    ~Study1Scene() override;
    void Init() override;
    void Update() override;
    void RenderingBegin() override;
    void RenderingEnd() override;

public:
    std::shared_ptr<Material> skyMaterial;
    std::shared_ptr<GameObject> camera;
    std::vector<std::shared_ptr<MeshRenderer>> boxMRs;
};
