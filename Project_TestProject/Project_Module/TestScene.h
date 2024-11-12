#pragma once
#include "Material.h"
#include "MeshRenderer.h"
#include "Scene.h"

class TestScene : public Scene
{
public:
    void* Clone() override;
    void ReRef() override;
    TestScene();
    TestScene(std::wstring name);
    virtual ~TestScene() override;
    virtual void Init() override;
    void Update() override;
    void RenderingBegin() override;
    void RenderingEnd() override;



public:
    std::shared_ptr<Material> skyMaterial;
    std::shared_ptr<GameObject> camera;
    std::vector<std::shared_ptr<MeshRenderer>> boxMRs;
};

