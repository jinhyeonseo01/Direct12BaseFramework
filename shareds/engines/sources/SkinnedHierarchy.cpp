#include "stdafx.h"
#include "SkinnedHierarchy.h"

#include "GameObject.h"

SkinnedHierarchy::SkinnedHierarchy()
{
}

SkinnedHierarchy::~SkinnedHierarchy()
{
}

void* SkinnedHierarchy::Clone()
{
    return RendererComponent::Clone();
}

void SkinnedHierarchy::ReRef()
{
    RendererComponent::ReRef();
}

void SkinnedHierarchy::Destroy()
{
    RendererComponent::Destroy();
}

void SkinnedHierarchy::Init()
{
    RendererComponent::Init();
}

void SkinnedHierarchy::Start()
{
    RendererComponent::Start();
    std::vector<std::shared_ptr<GameObject>> gameObjects;
    gameObjects.reserve(256);
    gameObject.lock()->GetChildsAll(gameObjects);

    for (auto& obj : gameObjects)
        nameToGameObject.emplace(obj->name, obj->transform);
}

void SkinnedHierarchy::Update()
{
    RendererComponent::Update();
}

void SkinnedHierarchy::LateUpdate()
{
    RendererComponent::LateUpdate();
}

void SkinnedHierarchy::OnEnable()
{
    RendererComponent::OnEnable();
}

void SkinnedHierarchy::OnDisable()
{
    RendererComponent::OnDisable();
}

void SkinnedHierarchy::OnDestroy()
{
    RendererComponent::OnDestroy();
}

void SkinnedHierarchy::OnComponentDestroy()
{
    RendererComponent::OnComponentDestroy();
}

void SkinnedHierarchy::BeforeRendering()
{
    RendererComponent::BeforeRendering();
}

void SkinnedHierarchy::Rendering(const RenderPacket& renderPack)
{
    RendererComponent::Rendering(renderPack);
}

void SkinnedHierarchy::AfterRendering()
{
    RendererComponent::AfterRendering();
}
