#include "stdafx.h"
#include "Collider.h"

#include "CBuffer_struct.h"
#include "GameObject.h"
#include "GraphicManager.h"
#include "Mesh.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Transform.h"


BoundingBox Collider::GetAABBBound()
{
    return BoundingBox();
}

void Collider::SetCollition(Collision collision)
{
    this->collision = collision;
}

Collider::~Collider()
{
}

void* Collider::Clone()
{
    return Component::Clone();
}

void Collider::ReRef()
{
    Component::ReRef();
}

void Collider::Destroy()
{
    Component::Destroy();
}

void Collider::Init()
{
    Component::Init();
    //aabbBox.GetCorners()
}

void Collider::Start()
{
    Component::Start();
    material = std::make_shared<Material>();
    material->shader = ResourceManager::main->GetShader(L"wireframe");
    material->SetData("_Color",
        Vector4(0,1,0,1));
}

void Collider::Update()
{
    Component::Update();
}

void Collider::LateUpdate()
{
    Component::LateUpdate();
}

void Collider::OnEnable()
{
    Component::OnEnable();
}

void Collider::OnDisable()
{
    Component::OnDisable();
}

void Collider::OnDestroy()
{
    Component::OnDestroy();
}

void Collider::OnComponentDestroy()
{
    Component::OnComponentDestroy();
}

void Collider::BeforeRendering()
{
    Component::BeforeRendering();

    std::shared_ptr<Mesh> mesh;
    if (collision.type == CollisionType::Box)
        mesh = ResourceManager::main->GetModel(L"Cube")->_meshList[0];
    if (collision.type == CollisionType::Sphere)
        mesh = ResourceManager::main->GetModel(L"Sphere")->_meshList[0];
    RenderPacket pack(mesh, material, std::dynamic_pointer_cast<Component>(this->shared_from_this()),
        Vector3::Distance(Vector3(SceneManager::GetCurrentScene()->_cameraParams.cameraPos), gameObject.lock()->transform->worldPosition()));
    SceneManager::GetCurrentScene()->AddRenderPacket(pack);

}

void Collider::Rendering(const RenderPacket& renderPack)
{
    Component::Rendering(renderPack);


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

    list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    list->IASetVertexBuffers(0, 1, &mesh->_vertexBufferView);
    list->IASetIndexBuffer(&mesh->_indexBufferView);


    auto cbuffer = pool->PopCBuffer("TransformParams");
    TransformParams data;
    gameObject.lock()->transform->GetLocalToWorldMatrix(data.WorldMatrix);

    if (collision.type == CollisionType::Box)
        data.WorldMatrix = Matrix::CreateScale(collision.boxBound.Extents) * Matrix::CreateTranslation(collision.boxBound.Center) * data.WorldMatrix;
    if (collision.type == CollisionType::Sphere)
        data.WorldMatrix = Matrix::CreateScale(collision.sphereBound.Radius*2) * Matrix::CreateTranslation(collision.sphereBound.Center) * data.WorldMatrix;

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


    list->DrawIndexedInstanced(mesh->indexCount, 1, 0, 0, 0);
    //list->DrawInstanced(mesh->vertexCount, 1, 0, 0);

    table->SetNextGroupHandle();
}

void Collider::AfterRendering()
{
    Component::AfterRendering();
}
