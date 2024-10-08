#pragma once
#include "RendererComponent.h"

namespace dxe
{
    class MeshRenderer : public xe::RendererComponent
    {
    public:
        ~MeshRenderer() override;
        void* Clone() const override;
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
    };
}

