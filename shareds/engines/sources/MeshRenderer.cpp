#include "stdafx.h"
#include "MeshRenderer.h"

#include "CBuffer_struct.h"
#include "Draw.h"
#include "DXEngine.h"
#include "GameObject.h"
#include "GraphicManager.h"
#include "Mesh.h"
#include "Model.h"
#include "Scene.h"
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

    for (int i = 0; i < materialList.size(); i++)
    {
        if (meshList.size() == 0)
            break;
        auto material = materialList[i]; //.lock()
        auto mesh = meshList[i % meshList.size()].lock();
        if (material == nullptr)
            continue;
        if (mesh == nullptr)
            continue;
        RenderPacket pack( mesh,material, std::bind(&MeshRenderer::Rendering, this, std::placeholders::_1),
            Vector3::Distance(Vector3(SceneManager::GetCurrentScene()->_cameraParams.cameraPos), gameObject.lock()->transform->worldPosition()));
        SceneManager::GetCurrentScene()->AddRenderPacket(pack);


        if (Engine::GetMainEngine()->_debugGizmo)
        {
            material = std::make_shared<Material>();
            material->shader = ResourceManager::main->GetShader(L"wireframe");
            material->SetData("_Color",
                Vector4(0, 0, 0, 1));

            RenderPacket pack2(mesh, material, std::bind(&MeshRenderer::Rendering, this, std::placeholders::_1),
                Vector3::Distance(Vector3(SceneManager::GetCurrentScene()->_cameraParams.cameraPos), gameObject.lock()->transform->worldPosition()));
            pack2.SetLifeExtension(material);
            SceneManager::GetCurrentScene()->AddRenderPacket(pack2);
        }
    }
}

void MeshRenderer::Rendering(const RenderPacket& renderPack)
{
    RendererComponent::Rendering(renderPack);
    
    auto material = renderPack.material.lock(); //.lock()
    auto mesh = renderPack.mesh.lock();
    if (!material)
        return;
    if (!mesh)
        return;

    auto list = GraphicManager::main->GetCurrentCommandList();
    auto pool = GraphicManager::main->GetCurrentCBufferPool();
    auto table = GraphicManager::main->GetCurrentDescriptorTable();

    material->shader.lock()->SetPipeline(list);

    if (isTess)
        list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
    else
        list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    list->IASetVertexBuffers(0, 1, &mesh->_vertexBufferView);
    list->IASetIndexBuffer(&mesh->_indexBufferView);

    if (material->shader.lock()->_info._stencilTest)
    {
        list->OMSetStencilRef(material->shader.lock()->_info._stencilIndex);

        Matrix refM = Matrix::Identity;
        material->GetData("ref", refM);
        auto cbuffer = pool->PopCBuffer("ReflectParams", sizeof(DefaultMaterialParams), 1024);
        ReflectParams data;
        data.ReflectMatrix = refM;
        cbuffer.SetData(&data, sizeof(data));
        table->SetCurrentGroupHandle(material->shader.lock(), "ReflectParams", cbuffer.handle);
    }
    else
        list->OMSetStencilRef(0);

    auto cbuffer = pool->PopCBuffer("TransformParams");
    TransformParams data;
    gameObject.lock()->transform->GetLocalToWorldMatrix(data.WorldMatrix);
    data.NormalMatrix = data.WorldMatrix.Invert().Transpose();
    data.isSkinned = 0;
    cbuffer.SetData(&data, sizeof(data));
    table->SetCurrentGroupHandle(material->shader.lock(), "TransformParams", cbuffer.handle);


    auto cbuffer2 = pool->PopCBuffer("DefaultMaterialParams", sizeof(DefaultMaterialParams), 512);
    DefaultMaterialParams data2;
    material->GetTextureDatas(table, material->shader.lock());
    material->GetData("_Color", data2.color);

    cbuffer2.SetData(&data2, sizeof(data2));
    table->SetCurrentGroupHandle(material->shader.lock(), "DefaultMaterialParams", cbuffer2.handle);


    GraphicManager::main->GetCurrentDescriptorTable()->RecycleCurrentGroupHandle(material->shader.lock(), "CameraParams");
    GraphicManager::main->GetCurrentDescriptorTable()->RecycleCurrentGroupHandle(material->shader.lock(), "MainLightParams");

    auto currentGroupStartGPuHandle = table->GetCurrentGroupGPUHandle(0);
    list->SetGraphicsRootDescriptorTable(1, currentGroupStartGPuHandle);


    list->DrawIndexedInstanced(mesh->indexCount, 1, 0, 0, 0);
    //list->DrawInstanced(mesh->vertexCount, 1, 0, 0);

    table->SetNextGroupHandle();
    
}

void MeshRenderer::AfterRendering()
{
    RendererComponent::AfterRendering();
}
