#include "stdafx.h"
#include "RendererComponent.h"

dxe::RendererComponent::RendererComponent()
{
}

dxe::RendererComponent::~RendererComponent()
{
}

void* dxe::RendererComponent::Clone()
{
    return Component::Clone();
}

void dxe::RendererComponent::ReRef()
{
    Component::ReRef();
}

void dxe::RendererComponent::Destroy()
{
    Component::Destroy();
}

void dxe::RendererComponent::Init()
{
    Component::Init();
}

void dxe::RendererComponent::Start()
{
    Component::Start();
}

void dxe::RendererComponent::Update()
{
    Component::Update();
}

void dxe::RendererComponent::LateUpdate()
{
    Component::LateUpdate();
}

void dxe::RendererComponent::OnEnable()
{
    Component::OnEnable();
}

void dxe::RendererComponent::OnDisable()
{
    Component::OnDisable();
}

void dxe::RendererComponent::OnDestroy()
{
    Component::OnDestroy();
}

void dxe::RendererComponent::OnComponentDestroy()
{
    Component::OnComponentDestroy();
}

void RendererComponent::BeforeRendering()
{
    Component::BeforeRendering();
}

void RendererComponent::Rendering()
{
    Component::Rendering();
}

void RendererComponent::AfterRendering()
{
    Component::AfterRendering();
}

void RendererComponent::AddMesh(std::vector<std::shared_ptr<Mesh>> meshList)
{
    for (auto& mesh : meshList)
        this->meshList.push_back(mesh);
}

void RendererComponent::AddMateiral(std::vector<std::shared_ptr<Material>> materialList)
{
    for (auto& material : materialList)
        this->materialList.push_back(material);
}

void RendererComponent::SetModel(std::shared_ptr<Model> model)
{
    this->model = model;
}
