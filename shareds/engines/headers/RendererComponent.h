#pragma once
#include "Component.h"

namespace dxe
{
    class RendererComponent : public Component
    {
    public:
        RendererComponent();
        virtual ~RendererComponent();
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
    };
}

