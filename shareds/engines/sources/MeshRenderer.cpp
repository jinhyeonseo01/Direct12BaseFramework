#include "MeshRenderer.h"

#include "GameObject.h"
#include "GraphicManager.h"
#include "Mesh.h"
#include "Transform.h"

MeshRenderer::MeshRenderer()
{
}

MeshRenderer::~MeshRenderer()
{
    
}

void* MeshRenderer::Clone()
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

    std::shared_ptr<Material> material = std::make_shared<Material>();
    material->shader = GraphicManager::instance->shaderList[0];

    texture = Texture::Load(L"test.png", true);
    material->SetData("test", texture);
    material->SetData("color", Vector4(0, 0, 1, 1));
    materials.push_back(material);

    mesh = std::make_shared<Mesh>();

    Vertex vert;
    vert.position = Vector3(-0.3, 0.3, 0.1f);
    vert.uvs[0] = Vector3(0, 0, 0);
    vert.color = Vector4(1, 1, 1, 1);
    vert.normal = Vector3(0, 0, 1);
    mesh->_vertexList.push_back(vert);
    vert.position = Vector3(0.3, 0.3, 0.1f);
    vert.uvs[0] = Vector3(1, 0, 0);
    vert.color = Vector4(1, 1, 1, 1);
    vert.normal = Vector3(0, 0, 1);
    mesh->_vertexList.push_back(vert);
    vert.position = Vector3(0.3, -0.3, 0.1f);
    vert.uvs[0] = Vector3(1, 1, 0);
    vert.color = Vector4(1, 1, 1, 1);
    vert.normal = Vector3(0, 0, 1);
    mesh->_vertexList.push_back(vert);
    vert.position = Vector3(-0.3, -0.3, 0.1f);
    vert.uvs[0] = Vector3(0, 1, 0);
    vert.color = Vector4(1, 1, 1, 1);
    vert.normal = Vector3(0, 0, 1);
    mesh->_vertexList.push_back(vert);

    mesh->_indexBuffer.push_back(0);
    mesh->_indexBuffer.push_back(1);
    mesh->_indexBuffer.push_back(2);
    mesh->_indexBuffer.push_back(0);
    mesh->_indexBuffer.push_back(2);
    mesh->_indexBuffer.push_back(3);

    mesh->CreateVertexBuffer();
    mesh->CreateIndexBuffer();
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

void MeshRenderer::BeforeRendering()
{
    RendererComponent::BeforeRendering();



}

void MeshRenderer::Rendering()
{
    RendererComponent::Rendering();


    for(int i=0;i< materials.size();i++)
    {
        auto& material = materials[i];
        auto list = GraphicManager::instance->GetCurrentCommandList();
        auto pool = GraphicManager::instance->GetCurrentCBufferPool();
        auto table = GraphicManager::instance->GetCurrentDescriptorTable();

        material->shader.lock()->SetPipeline(list);

        list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        list->IASetVertexBuffers(0, 1, &mesh->_vertexBufferView);
        list->IASetIndexBuffer(&mesh->_indexBufferView);


        auto cbuffer = pool->PopCBuffer("TransformParams");
        TransformParams data;
        gameObject.lock()->transform->GetLocalToWorldMatrix(data.WorldMatrix);
        cbuffer.SetData(&data, sizeof(data));
        table->SetCurrentGroupHandle(material->shader.lock(), "TransformParams", cbuffer.handle);


        auto cbuffer2 = pool->PopCBuffer("DefaultMaterialParams", sizeof(DefaultMaterialParams));
        DefaultMaterialParams data2;
        material->SetTextureDatas(table, material->shader.lock());
        material->GetData("color", data2.color);

        cbuffer2.SetData(&data2, sizeof(data2));
        table->SetCurrentGroupHandle(material->shader.lock(), "DefaultMaterialParams", cbuffer2.handle);

        GraphicManager::instance->GetCurrentDescriptorTable()->RecycleCurrentGroupHandle(material->shader.lock(), "CameraParams");

        auto a = table->GetCurrentGroupGPUHandle(0);
        list->SetGraphicsRootDescriptorTable(1, a);


        list->DrawIndexedInstanced(mesh->indexCount, 1, 0, 0, 0);
        //list->DrawInstanced(mesh->vertexCount, 1, 0, 0);

        table->SetNextGroupHandle();
    }
}

void MeshRenderer::AfterRendering()
{
    RendererComponent::AfterRendering();
}
