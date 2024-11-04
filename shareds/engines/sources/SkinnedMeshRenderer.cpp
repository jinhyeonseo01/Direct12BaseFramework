#include "stdafx.h"
#include "SkinnedMeshRenderer.h"

#include "GameObject.h"
#include "GraphicManager.h"
#include "Mesh.h"
#include "Model.h"
#include "Transform.h"


void* SkinnedMeshRenderer::Clone()
{
    return RendererComponent::Clone();
}

void SkinnedMeshRenderer::ReRef()
{
    RendererComponent::ReRef();
}

void SkinnedMeshRenderer::Destroy()
{
    RendererComponent::Destroy();
}

void SkinnedMeshRenderer::Init()
{
    RendererComponent::Init();

    std::shared_ptr<Material> material = std::make_shared<Material>();
    material->shader = ResourceManager::main->GetShader(L"forward");;

    texture = Texture::Load(L"test.png", true);
    material->SetData("test", texture);
    material->SetData("color", Vector4(0, 0, 1, 1));
    materials.push_back(material);
}

void SkinnedMeshRenderer::Start()
{
    RendererComponent::Start();
}

void SkinnedMeshRenderer::Update()
{
    RendererComponent::Update();
}

void SkinnedMeshRenderer::LateUpdate()
{
    RendererComponent::LateUpdate();
}

void SkinnedMeshRenderer::OnEnable()
{
    RendererComponent::OnEnable();
}

void SkinnedMeshRenderer::OnDisable()
{
    RendererComponent::OnDisable();
}

void SkinnedMeshRenderer::OnDestroy()
{
    RendererComponent::OnDestroy();
}

void SkinnedMeshRenderer::OnComponentDestroy()
{
    RendererComponent::OnComponentDestroy();
}

void SkinnedMeshRenderer::BeforeRendering()
{
    RendererComponent::BeforeRendering();



}

void SkinnedMeshRenderer::Rendering()
{
    RendererComponent::Rendering();


    for (int i = 0; i < materials.size(); i++)
    {
        auto& material = materials[i];
        auto list = GraphicManager::main->GetCurrentCommandList();
        auto pool = GraphicManager::main->GetCurrentCBufferPool();
        auto table = GraphicManager::main->GetCurrentDescriptorTable();

        material->shader.lock()->SetPipeline(list);

        list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        list->IASetVertexBuffers(0, 1, &meshList[i]->_vertexBufferView);
        list->IASetIndexBuffer(&meshList[i]->_indexBufferView);


        auto cbuffer = pool->PopCBuffer("TransformParams");
        TransformParams data;
        gameObject.lock()->transform->GetLocalToWorldMatrix(data.WorldMatrix);
        data.isSkinned = true;
        cbuffer.SetData(&data, sizeof(data));
        table->SetCurrentGroupHandle(material->shader.lock(), "TransformParams", cbuffer.handle);


        auto cbuffer2 = pool->PopCBuffer("DefaultMaterialParams", sizeof(DefaultMaterialParams), 512);
        DefaultMaterialParams data2;
        material->SetTextureDatas(table, material->shader.lock());
        material->GetData("color", data2.color);

        cbuffer2.SetData(&data2, sizeof(data2));
        table->SetCurrentGroupHandle(material->shader.lock(), "DefaultMaterialParams", cbuffer2.handle);


        auto cbuffer3 = pool->PopCBuffer("BoneParams", sizeof(BoneParams), 32);
        BoneParams data3;
        //model
        std::vector<std::shared_ptr<GameObject>> objs;
        Matrix mat;
        objs.reserve(256);
        for (int j = 0; j < model->_boneList.size(); j++)
        {
            objs.clear();
            gameObject.lock()->rootParent.lock()->GetChildsAllByName(std::to_wstring(model->_boneList[j]->nodeName), objs);
            objs[0]->transform->GetLocalToWorldMatrix(mat);
            data3.boneMatrix[j] = model->_boneList[j]->offsetTransform * mat; // * model->_boneList[j]->offsetTransform
            //std::memcpy(&(data3.boneMatrix[j]._11), &(mat._11), sizeof(mat));
        }
        cbuffer3.SetData(&data3, sizeof(data3));
        table->SetCurrentGroupHandle(material->shader.lock(), "BoneParams", cbuffer3.handle);

        GraphicManager::main->GetCurrentDescriptorTable()->RecycleCurrentGroupHandle(material->shader.lock(), "CameraParams");

        auto a = table->GetCurrentGroupGPUHandle(0);
        list->SetGraphicsRootDescriptorTable(1, a);


        list->DrawIndexedInstanced(meshList[i]->indexCount, 1, 0, 0, 0);
        //list->DrawInstanced(mesh->vertexCount, 1, 0, 0);

        table->SetNextGroupHandle();
    }
}

void SkinnedMeshRenderer::AfterRendering()
{
    RendererComponent::AfterRendering();
}
