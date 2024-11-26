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

CameraParams Camera::GetCameraParams()
{
    CameraParams params;

    cameraInfo.cameraScreenData = Vector4(
        GraphicManager::main->setting.screenInfo.width,
        GraphicManager::main->setting.screenInfo.height,
        GraphicManager::main->setting.screenInfo.x,
        GraphicManager::main->setting.screenInfo.y);
    cameraInfo.cameraFrustumData = Vector4(_fovy * D2R, cameraInfo.cameraScreenData.x / cameraInfo.cameraScreenData.y, _near, _far);

    auto worldPosition = gameObject.lock()->transform->worldPosition();
    auto worldDirection = gameObject.lock()->transform->forward();
    auto worldUp = gameObject.lock()->transform->up();

    cameraInfo.cameraPos = Vector4(worldPosition.x, worldPosition.y, worldPosition.z, 1);
    cameraInfo.cameraDirection = Vector4(worldDirection.x, worldDirection.y, worldDirection.z, 0);
    cameraInfo.cameraUp = Vector4(worldUp.x, worldUp.y, worldUp.z, 0);

    cameraInfo.projectionMatrix = Matrix::CreatePerspectiveFieldOfView(
        cameraInfo.cameraFrustumData.x, 
        cameraInfo.cameraFrustumData.y,
        cameraInfo.cameraFrustumData.z, 
        cameraInfo.cameraFrustumData.w);

    cameraInfo.viewMatrix = XMMatrixLookToLH(
        cameraInfo.cameraPos,
        cameraInfo.cameraDirection,
        cameraInfo.cameraUp);
    cameraInfo.VPMatrix = cameraInfo.viewMatrix * cameraInfo.projectionMatrix;

    cameraInfo.InvertViewMatrix = cameraInfo.viewMatrix.Invert();
    cameraInfo.InvertProjectionMatrix = cameraInfo.projectionMatrix.Invert();
    cameraInfo.InvertVPMatrix = cameraInfo.VPMatrix.Invert();

    return cameraInfo;
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
