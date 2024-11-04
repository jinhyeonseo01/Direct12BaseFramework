#include "stdafx.h"
#include "MeshRenderer.h"

#include "GameObject.h"
#include "GraphicManager.h"
#include "Mesh.h"
#include "Model.h"
#include "SceneManager.h"
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


    for(int i=0;i< materialList.size();i++)
    {
        auto material = materialList[i];//.lock()
        auto mesh = meshList[i % meshList.size()].lock();
        if (!material)
            continue;
        if (!mesh)
            continue;

        auto list = GraphicManager::main->GetCurrentCommandList();
        auto pool = GraphicManager::main->GetCurrentCBufferPool();
        auto table = GraphicManager::main->GetCurrentDescriptorTable();

        material->shader.lock()->SetPipeline(list);

        list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        list->IASetVertexBuffers(0, 1, &mesh->_vertexBufferView);
        list->IASetIndexBuffer(&mesh->_indexBufferView);


        auto cbuffer = pool->PopCBuffer("TransformParams");
        TransformParams data;
        gameObject.lock()->transform->GetLocalToWorldMatrix(data.WorldMatrix);
        data.isSkinned = 0;
        cbuffer.SetData(&data, sizeof(data));
        table->SetCurrentGroupHandle(material->shader.lock(), "TransformParams", cbuffer.handle);


        auto cbuffer2 = pool->PopCBuffer("DefaultMaterialParams", sizeof(DefaultMaterialParams), 512);
        DefaultMaterialParams data2;
        material->SetTextureDatas(table, material->shader.lock());
        material->GetData("color", data2.color);

        cbuffer2.SetData(&data2, sizeof(data2));
        table->SetCurrentGroupHandle(material->shader.lock(), "DefaultMaterialParams", cbuffer2.handle);

        
        GraphicManager::main->GetCurrentDescriptorTable()->RecycleCurrentGroupHandle(material->shader.lock(), "CameraParams");

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
