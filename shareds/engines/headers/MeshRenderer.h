#pragma once
#include "stdafx.h"
#include "Material.h"
#include "RendererComponent.h"

namespace dxe
{
    class MeshRenderer : public dxe::RendererComponent
    {
    public:
        MeshRenderer();
        ~MeshRenderer() override;
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

        Vector3 pos;
    };
}
