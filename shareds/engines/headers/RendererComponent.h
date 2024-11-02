#pragma once
#include "Component.h"
#include "Material.h"

namespace dxe
{
    class RendererComponent : public Component
    {
    public:
        RendererComponent();
        virtual ~RendererComponent() override;
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
        std::vector<std::shared_ptr<Material>> materialList;
        std::vector<std::weak_ptr<Mesh>> meshList;
        std::weak_ptr<Model> model;

        void AddMesh(std::vector<std::shared_ptr<Mesh>> meshList);
        void AddMateiral(std::vector<std::shared_ptr<Material>> materialList);
        void SetModel(std::shared_ptr<Model> model);
    };
}

