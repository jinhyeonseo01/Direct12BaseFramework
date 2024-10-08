#include "RendererComponent.h"

xe::RendererComponent::RendererComponent()
{
}

xe::RendererComponent::~RendererComponent()
{
}

void* xe::RendererComponent::Clone() const
{
    return Component::Clone();
}

void xe::RendererComponent::ReRef()
{
    Component::ReRef();
}

void xe::RendererComponent::Destroy()
{
    Component::Destroy();
}

void xe::RendererComponent::Init()
{
    Component::Init();
}

void xe::RendererComponent::Start()
{
    Component::Start();
}

void xe::RendererComponent::Update()
{
    Component::Update();
}

void xe::RendererComponent::LateUpdate()
{
    Component::LateUpdate();
}

void xe::RendererComponent::OnEnable()
{
    Component::OnEnable();
}

void xe::RendererComponent::OnDisable()
{
    Component::OnDisable();
}

void xe::RendererComponent::OnDestroy()
{
    Component::OnDestroy();
}

void xe::RendererComponent::OnComponentDestroy()
{
    Component::OnComponentDestroy();
}
