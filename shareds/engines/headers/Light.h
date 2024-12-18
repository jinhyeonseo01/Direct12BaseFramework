#pragma once
#include "Camera.h"
#include "Component.h"


namespace dxe
{
    enum class LightType
    {
        Directional,
        Point,
        Spot
    };
    class Light : public Camera
    {
    protected:
        static std::weak_ptr<Light> main;
    public:
        static std::shared_ptr<Light> GetMainShadowLight();
        static void SetMainShadowLight(std::shared_ptr<Light> light);
        LightType type;

    public:
        ~Light() override;
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
    };
}

