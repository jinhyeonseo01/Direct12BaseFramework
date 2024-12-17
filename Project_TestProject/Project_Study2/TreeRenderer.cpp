#include "TreeRenderer.h"

#include "CBuffer_struct.h"
#include "GameObject.h"
#include "GraphicManager.h"
#include "Mesh.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Transform.h"

TreeRenderer::~TreeRenderer()
{
}

void* TreeRenderer::Clone()
{
    return RendererComponent::Clone();
}

void TreeRenderer::ReRef()
{
    RendererComponent::ReRef();
}

void TreeRenderer::Destroy()
{
    RendererComponent::Destroy();
}

void TreeRenderer::Init()
{
    RendererComponent::Init();

}

void TreeRenderer::Start()
{
    RendererComponent::Start();

    std::vector<std::shared_ptr<GameObject>> targets;
    targets.clear();
    gameObject.lock()->scene.lock()->Finds(L"B2", targets, false);
    auto t = targets[0];
    targets.clear();
    t->GetChildsAllByName(L"default", targets);
    ter = targets[0];

    hmT = ResourceManager::main->GetTexture(L"hm");


    points = std::make_shared<Mesh>();
    std::vector<Vertex> pointList;
    Vertex vert;


    for (int i = 0; i < count; i++)
    {
        auto localPos = ter.lock()->transform->WorldToLocal_Position(Vector3((rand() % 1000) / 1000.0 * 1000.0 - 500.0, 0, (rand() % 1000) / 1000.0 * 1000.0 - 500.0));
        Vector4 color = hmT.lock()->GetPixel(
            (std::round(std::abs(localPos.x)) / 100) * hmT.lock()->_size.x,
            (1 - std::round(std::abs(localPos.z)) / 100) * hmT.lock()->_size.y, 1
        );
        localPos.y = (color.x * 100);

        auto worldPos = ter.lock()->transform->LocalToWorld_Position(localPos);
        vert.position = worldPos;
        pointList.push_back(vert);
    }
    points->Init(pointList);
    points->CreateVertexBuffer();
}

void TreeRenderer::Update()
{
    RendererComponent::Update();
}

void TreeRenderer::LateUpdate()
{
    RendererComponent::LateUpdate();
}

void TreeRenderer::OnEnable()
{
    RendererComponent::OnEnable();
}

void TreeRenderer::OnDisable()
{
    RendererComponent::OnDisable();
}

void TreeRenderer::OnDestroy()
{
    RendererComponent::OnDestroy();
}

void TreeRenderer::OnComponentDestroy()
{
    RendererComponent::OnComponentDestroy();
}

void TreeRenderer::BeforeRendering()
{
    RendererComponent::BeforeRendering();

    for (int i = 0; i < materialList.size(); i++)
    {
        auto material = materialList[i]; //.lock()
        if (!material)
            continue;
        RenderPacket pack(nullptr, material, std::bind(&TreeRenderer::Rendering, this, std::placeholders::_1),
            Vector3::Distance(Vector3(SceneManager::GetCurrentScene()->_cameraParams.cameraPos), gameObject.lock()->transform->worldPosition()));
        SceneManager::GetCurrentScene()->AddRenderPacket(pack);
    }

}

void TreeRenderer::Rendering(const RenderPacket& renderPack)
{
    RendererComponent::Rendering(renderPack);

    auto list = GraphicManager::main->GetCurrentCommandList();
    auto pool = GraphicManager::main->GetCurrentCBufferPool();
    auto table = GraphicManager::main->GetCurrentDescriptorTable();

    auto material = renderPack.material.lock(); //.lock()
    auto mesh = renderPack.mesh.lock();
    if (!material)
        return;

    material->shader.lock()->SetPipeline(list);

    list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
    list->IASetVertexBuffers(0, 1, &points->_vertexBufferView);


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


    GraphicManager::main->GetCurrentDescriptorTable()->RecycleCurrentGroupHandle(
        material->shader.lock(), "CameraParams");

    auto a = table->GetCurrentGroupGPUHandle(0);
    list->SetGraphicsRootDescriptorTable(1, a);


    list->DrawInstanced(points->vertexCount, 1, 0, 0);
    //list->DrawIndexedInstanced(mesh->indexCount, 1, 0, 0, 0);
    //list->DrawInstanced(mesh->vertexCount, 1, 0, 0);

    table->SetNextGroupHandle();
}

void TreeRenderer::AfterRendering()
{
    RendererComponent::AfterRendering();
}
