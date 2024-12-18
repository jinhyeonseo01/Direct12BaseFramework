#include "stdafx.h"
#include "Light.h"


std::weak_ptr<Light> Light::main = std::weak_ptr<Light>();

std::shared_ptr<Light> Light::GetMainShadowLight()
{
    return main.lock();
}

void Light::SetMainShadowLight(std::shared_ptr<Light> light)
{
    main = light;
}

Light::~Light()
{

}

void* Light::Clone()
{
    return Camera::Clone();
}

void Light::ReRef()
{
    Camera::ReRef();
}

void Light::Destroy()
{
    Camera::Destroy();
}

void Light::Init()
{
    Camera::Init();

    if (GetMainShadowLight() == nullptr)
        SetMainShadowLight(std::dynamic_pointer_cast<Light>(shared_from_this()));

    projectionMode = CameraProjectionMode::Orthographic;
}

void Light::Start()
{
    Camera::Start();
}

void Light::Update()
{
    Camera::Update();
}

void Light::LateUpdate()
{
    Camera::LateUpdate();
}

void Light::OnEnable()
{
    Camera::OnEnable();
}

void Light::OnDisable()
{
    Camera::OnDisable();
}

void Light::OnDestroy()
{
    Camera::OnDestroy();
}

void Light::OnComponentDestroy()
{
    Camera::OnComponentDestroy();
}

void Light::BeforeRendering()
{
    Camera::BeforeRendering();
}

void Light::Rendering(const RenderPacket& renderPack)
{
    Camera::Rendering(renderPack);
}

void Light::AfterRendering()
{
    Camera::AfterRendering();
}
