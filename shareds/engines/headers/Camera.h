#pragma once
#include "Component.h"

namespace dxe
{
    class Camera : public Component
    {
    public:
        Camera();
        virtual ~Camera();

    public:
        float _near;
        float _far;
        float _fovy;

    public:

        void* Clone() const override;
        void ReRef() const override;
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

