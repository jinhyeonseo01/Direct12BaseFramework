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

Vector3 Camera::GetScreenToWorldPosition(Vector2 mousePosition)
{
    auto cameraParam = GetCameraParams();
    auto screenToWorld = CreateViewportMatrix(Engine::GetMainEngine()->GetWindowRect()).Invert() * cameraParam.InvertVPMatrix;
    return Vector3::Transform(Vector3(mousePosition.x, mousePosition.y, 0), screenToWorld);
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

    cameraInfo.ProjectionMatrix = Matrix::CreatePerspectiveFieldOfView(
        cameraInfo.cameraFrustumData.x, 
        cameraInfo.cameraFrustumData.y,
        cameraInfo.cameraFrustumData.z, 
        cameraInfo.cameraFrustumData.w);

    cameraInfo.ViewMatrix = XMMatrixLookToLH(
        cameraInfo.cameraPos,
        cameraInfo.cameraDirection,
        cameraInfo.cameraUp);
    cameraInfo.VPMatrix = cameraInfo.ViewMatrix * cameraInfo.ProjectionMatrix;

    cameraInfo.InvertViewMatrix = cameraInfo.ViewMatrix.Invert();
    cameraInfo.InvertProjectionMatrix = cameraInfo.ProjectionMatrix.Invert();
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
    auto pos = GetScreenToWorldPosition(Input::main->GetMousePosition());
    Vector3 dir;
    (pos - gameObject.lock()->transform->worldPosition()).Normalize(dir);

    SceneManager::GetCurrentScene()->Find(L"Cube")->transform->worldPosition(pos + dir * 10); //
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

void Camera::Rendering(const RenderPacket& renderPack)
{
    Component::Rendering(renderPack);
}

void Camera::AfterRendering()
{
    Component::AfterRendering();
}
