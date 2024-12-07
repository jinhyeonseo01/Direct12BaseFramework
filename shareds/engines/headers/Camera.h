#pragma once
#include "CBuffer_struct.h"
#include "Component.h"
#include "GraphicSetting.h"

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

        CameraParams cameraInfo;

        Matrix _projectionMatrix;
        Matrix _viewMatrix;
        Matrix _viewProjectionMatrix;
        Matrix _ndcToScreenSpaceMatrix;

        Vector3 GetScreenToWorldDirection(Vector2 mousePosition);

    public:

        void* Clone() override;
        void ReRef() override;
        void Init() override;
        void Start() override;
        void Update() override;
        void LateUpdate() override;
        void OnEnable() override;
        void OnDisable() override;
        void OnDestroy() override;
        void OnComponentDestroy() override;
        void Destroy() override;
        void BeforeRendering() override;
        void Rendering() override;
        void AfterRendering() override;
    };
}

