#include "Camera.h"


Camera::Camera()
{
}

Camera::~Camera()
{
}

void* Camera::Clone() const
{
    return Component::Clone();
}

void Camera::ReRef() const
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
