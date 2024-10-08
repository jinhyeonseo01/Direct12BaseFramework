#include "MeshRenderer.h"

MeshRenderer::~MeshRenderer()
{
}

void* MeshRenderer::Clone() const
{
    return RendererComponent::Clone();
}

void MeshRenderer::ReRef()
{
    RendererComponent::ReRef();
}

void MeshRenderer::Destroy()
{
    RendererComponent::Destroy();
}

void MeshRenderer::Init()
{
    RendererComponent::Init();
}

void MeshRenderer::Start()
{
    RendererComponent::Start();
}

void MeshRenderer::Update()
{
    RendererComponent::Update();
}

void MeshRenderer::LateUpdate()
{
    RendererComponent::LateUpdate();
}

void MeshRenderer::OnEnable()
{
    RendererComponent::OnEnable();
}

void MeshRenderer::OnDisable()
{
    RendererComponent::OnDisable();
}

void MeshRenderer::OnDestroy()
{
    RendererComponent::OnDestroy();
}

void MeshRenderer::OnComponentDestroy()
{
    RendererComponent::OnComponentDestroy();
}
