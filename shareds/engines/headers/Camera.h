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
        float _near = 0.03f;
        float _far = 1000.0f;
        float _fovy = 60.0f;
        float _aspect = 1;

        Matrix _projection;
        void A()
        {
            Matrix::CreatePerspectiveFieldOfView(_fovy, _aspect, _near, _far);
        }

    public:

        void* Clone() const override;
        void ReRef() override;
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

