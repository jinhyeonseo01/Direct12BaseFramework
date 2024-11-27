#include "CameraController.h"

#include "DXEngine.h"
#include "GameObject.h"
#include "Input.h"

CameraController::~CameraController()
{
}

void* CameraController::Clone()
{
    return Component::Clone();
}

void CameraController::ReRef()
{
    Component::ReRef();
}

void CameraController::Destroy()
{
    Component::Destroy();
}

void CameraController::Init()
{
    Component::Init();
}

void CameraController::Start()
{
    Component::Start();

    camera = gameObject.lock()->GetComponent<Camera>();
}

void CameraController::Update()
{
    Component::Update();


    if (Input::main->GetMouseDown(KeyCode::RightMouse))
    {
        cameraControl = true;
        prevMourePosition = Input::main->GetMousePosition();
        Engine::GetMainEngine()->SetCursorHide(cameraControl);
    }
    if (Input::main->GetMouseUp(KeyCode::RightMouse))
    {
        cameraControl = false;
        Engine::GetMainEngine()->SetCursorHide(cameraControl);
    }
    if (cameraControl) //Input::main->GetMouse(KeyCode::RightMouse) && 
    {
        auto mouseDelta = (Input::main->GetMousePosition() - prevMourePosition);
        auto mouseDeltaAngle = (((Vector3(0, 1, 0) * mouseDelta.x) + (Vector3(1, 0, 0) * mouseDelta.y)) * 0.003f);

        totalAngle += mouseDeltaAngle;
        if (std::abs(totalAngle.x) >= (90 * D2R))
            totalAngle.x = std::sign(totalAngle.x) * 89.99f * D2R;

        gameObject.lock()->transform->localEuler(totalAngle);
        prevMourePosition = Input::main->GetMousePosition();
    }

    auto moveDirection = Vector3::Zero;
    moveDirection += gameObject.lock()->transform->up() * ((Input::main->GetKey(KeyCode::E) ? 1 : 0) - (
        Input::main->GetKey(KeyCode::Q) ? 1 : 0));
    moveDirection += gameObject.lock()->transform->right() * ((Input::main->GetKey(KeyCode::D) ? 1 : 0) - (
        Input::main->GetKey(KeyCode::A) ? 1 : 0));
    moveDirection += gameObject.lock()->transform->forward() * ((Input::main->GetKey(KeyCode::W) ? 1 : 0) - (
        Input::main->GetKey(KeyCode::S) ? 1 : 0));
    moveDirection.Normalize(moveDirection);

    float moveSpeed = 0.1f;
    gameObject.lock()->transform->worldPosition(
        gameObject.lock()->transform->worldPosition() + Engine::GetMainEngine()->deltaTime * 60 * moveDirection *
        moveSpeed);
}

void CameraController::LateUpdate()
{
    Component::LateUpdate();
}

void CameraController::OnEnable()
{
    Component::OnEnable();
}

void CameraController::OnDisable()
{
    Component::OnDisable();
}

void CameraController::OnDestroy()
{
    Component::OnDestroy();
}

void CameraController::OnComponentDestroy()
{
    Component::OnComponentDestroy();
}

void CameraController::BeforeRendering()
{
    Component::BeforeRendering();
}

void CameraController::Rendering(const RenderPacket& renderPack)
{
    Component::Rendering(renderPack);
}

void CameraController::AfterRendering()
{
    Component::AfterRendering();
}
