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

void* Camera::Clone()
{
    auto thisObject = GetThis<Camera>();
    auto cloneObject = std::make_shared<Camera>()->MakeInit<Camera>();
    AddClone(thisObject, cloneObject);

    cloneObject->gameObject = thisObject->gameObject;
    cloneObject->_first = thisObject->_first;
    {

    }

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

Vector2 prevPos;
Vector3 angle = Vector3::Zero;

void Camera::Update()
{
    Component::Update();

    if (Input::main->GetMouseDown(KeyCode::RightMouse))
        prevPos = Input::main->GetMousePosition();
    if (Input::main->GetMouse(KeyCode::RightMouse))
    {
        auto pos = (Input::main->GetMousePosition() - prevPos);
        auto movePos = -(((Vector3(0, 1, 0) * pos.x) + (Vector3(1, 0, 0) * pos.y)) * 0.01f);

        angle += movePos;

        gameObject.lock()->transform->localEuler(angle);
        //gameObject.lock()->transform->LookUp(movePos, Vector3(0, 1, 0));
        prevPos = Input::main->GetMousePosition();
    }

    auto r = gameObject.lock()->transform->right() * ((Input::main->GetKey(KeyCode::D) ? 1 : 0) - (Input::main->GetKey(KeyCode::A) ? 1 : 0));
    r += gameObject.lock()->transform->forward() * ((Input::main->GetKey(KeyCode::W) ? 1 : 0) - (Input::main->GetKey(KeyCode::S) ? 1 : 0));

    r.Normalize(r);
    gameObject.lock()->transform->worldPosition(gameObject.lock()->transform->worldPosition() + r * 0.01f);

    
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
    _aspect = GraphicManager::instance->setting.screenInfo.width / GraphicManager::instance->setting.screenInfo.height;

    cameraInfo.projectionMatrix = Matrix::CreatePerspectiveFieldOfView(_fovy*D2R, _aspect, _near, _far);
    cameraInfo.viewMatrix = XMMatrixLookToLH(gameObject.lock()->transform->worldPosition(),
        gameObject.lock()->transform->forward(),
        gameObject.lock()->transform->up());
    
    auto cameraBuffer = GraphicManager::instance->GetCurrentCBufferPool()->PopCBuffer("CameraParams", sizeof(CameraParams), 1);
    cameraBuffer.SetData(&cameraInfo, sizeof(CameraParams));
    
    GraphicManager::instance->GetCurrentDescriptorTable()->AddRecycleHandle("CameraParams", cameraBuffer.handle);
}

void Camera::Rendering()
{
    Component::Rendering();
}

void Camera::AfterRendering()
{
    Component::AfterRendering();
}
