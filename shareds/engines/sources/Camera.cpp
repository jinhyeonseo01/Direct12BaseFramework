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

Vector2 prevPos;
Vector3 angle = Vector3::Zero;
bool cameraControl = false;

void Camera::Update()
{
    Component::Update();

    if (Input::main->GetMouseDown(KeyCode::RightMouse))
    {
        cameraControl = true;
        prevPos = Input::main->GetMousePosition();
        //ShowCursor(cameraControl ? TRUE : FALSE);
        Engine::GetMainEngine()->SetCursorHide(cameraControl);
    }
    if (Input::main->GetMouseUp(KeyCode::RightMouse))
    {
        cameraControl = false;
        Engine::GetMainEngine()->SetCursorHide(cameraControl);
    }
    if (cameraControl)//Input::main->GetMouse(KeyCode::RightMouse) && 
    {
        auto pos = (Input::main->GetMousePosition() - prevPos);
        auto movePos = (((Vector3(0, 1, 0) * pos.x) + (Vector3(1, 0, 0) * pos.y)) * 0.003f);


        angle += movePos;
        if (std::abs(angle.x) >= (90 * D2R))
            angle.x = std::sign(angle.x) * 89.99f * D2R;

        gameObject.lock()->transform->localEuler(angle);
        //gameObject.lock()->transform->LookUp(movePos, Vector3(0, 1, 0));
        prevPos = Input::main->GetMousePosition();


        auto r = gameObject.lock()->transform->right() * ((Input::main->GetKey(KeyCode::D) ? 1 : 0) - (Input::main->GetKey(KeyCode::A) ? 1 : 0));
        r += gameObject.lock()->transform->forward() * ((Input::main->GetKey(KeyCode::W) ? 1 : 0) - (Input::main->GetKey(KeyCode::S) ? 1 : 0));

        r.Normalize(r);
        gameObject.lock()->transform->worldPosition(gameObject.lock()->transform->worldPosition() + Engine::GetMainEngine()->deltaTime *60 * r);
    }
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
