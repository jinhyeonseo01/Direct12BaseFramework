#include "stdafx.h"
#include "Camera.h"

#include "DXEngine.h"
#include "GameObject.h"
#include "GraphicManager.h"
#include "GraphicSetting.h"
#include "Transform.h"


Camera::Camera()
{
}

Camera::~Camera()
{
}

Vector3 Camera::GetScreenToWorldDirection(Vector2 mousePosition)
{
    //X = (X + 1) * Viewport.Width * 0.5 + Viewport.TopLeftX
    //Y = (1 - Y) * Viewport.Height * 0.5 + Viewport.TopLeftY
    //Z = Viewport.MinDepth + Z * (Viewport.MaxDepth - Viewport.MinDepth)
    return Vector3();
}

void* Camera::Clone()
{
    auto thisObject = GetThis<Camera>();
    auto cloneObject = std::make_shared<Camera>()->MakeInit<Camera>();
    AddClone(thisObject, cloneObject);

    cloneObject->gameObject = thisObject->gameObject;
    cloneObject->_first = thisObject->_first;

    return cloneObject.get();
}

void Camera::ReRef()
{
    Component::ReRef();
}

void Camera::Init()
{
    Component::Init();
}

void Camera::Start()
{
    Component::Start();
}

void Camera::Update()
{
    Component::Update();
}

void Camera::LateUpdate()
{
    Component::LateUpdate();
}

void Camera::OnEnable()
{
    Component::OnEnable();
}

void Camera::OnDisable()
{
    Component::OnDisable();
}

void Camera::OnDestroy()
{
    Component::OnDestroy();
}

void Camera::OnComponentDestroy()
{
    Component::OnComponentDestroy();
}

void Camera::Destroy()
{
    Component::Destroy();
}

void Camera::BeforeRendering()
{
    Component::BeforeRendering();
}

void Camera::Rendering()
{
    Component::Rendering();
}

void Camera::AfterRendering()
{
    Component::AfterRendering();
}
