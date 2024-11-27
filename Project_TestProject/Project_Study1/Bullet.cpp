#include "Bullet.h"

#include "DXEngine.h"

Bullet::~Bullet()
{
}

void* Bullet::Clone()
{
    return Component::Clone();
}

void Bullet::ReRef()
{
    Component::ReRef();
}

void Bullet::Destroy()
{
    Component::Destroy();
}

void Bullet::Init()
{
    Component::Init();
}

void Bullet::Start()
{
    Component::Start();
}

void Bullet::Update()
{
    Component::Update();

    time -= Engine::GetMainEngine()->deltaTime;
    gameObject.lock()->transform->worldPosition(gameObject.lock()->transform->worldPosition() +
        gameObject.lock()->transform->forward() * Engine::GetMainEngine()->deltaTime * 60 * 5);
    if (time < 0)
    {
        Destroy();
    }
}

void Bullet::LateUpdate()
{
    Component::LateUpdate();
}

void Bullet::OnEnable()
{
    Component::OnEnable();
}

void Bullet::OnDisable()
{
    Component::OnDisable();
}

void Bullet::OnDestroy()
{
    Component::OnDestroy();
}

void Bullet::OnComponentDestroy()
{
    Component::OnComponentDestroy();
}

void Bullet::BeforeRendering()
{
    Component::BeforeRendering();
}

void Bullet::Rendering(const RenderPacket& renderPack)
{
    Component::Rendering(renderPack);
}

void Bullet::AfterRendering()
{
    Component::AfterRendering();
}
