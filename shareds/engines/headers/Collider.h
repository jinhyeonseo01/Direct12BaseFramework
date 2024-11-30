#pragma once
#include <stdafx.h>
#include "Component.h"
#include <Collision.h>

namespace dxe
{

    class Collider : public Component
    {
    private:
    public:
        Collision collision;

        BoundingBox GetAABBBound();
        void SetCollition(Collision collision);

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
        void Rendering(const RenderPacket& renderPack) override;
        void AfterRendering() override;
    public:
        std::shared_ptr<Material> material;
    };


}
