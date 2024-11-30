#include "stdafx.h"
#include "Draw.h"

#include "GraphicManager.h"
#include "MeshRenderer.h"
#include "Scene.h"
#include "SceneManager.h"


void Draw::Release()
{
}

void Draw::Ray(const SimpleMath::Ray& ray, const Color& color)
{
    std::shared_ptr<Material> material = std::make_shared<Material>();
    material->SetShader(ResourceManager::main->GetShader(L"draw_ray"));
    material->SetData("pos0", ray.position);
    material->SetData("pos1", ray.position + ray.direction);
    material->SetData("_Color", Vector4(color));


    RenderPacket pack(nullptr, material, [=](const RenderPacket& render)
    {
        auto material = render.material.lock(); //.lock()
        if (material == nullptr)
            return;

        auto list = GraphicManager::main->GetCurrentCommandList();
        auto pool = GraphicManager::main->GetCurrentCBufferPool();
        auto table = GraphicManager::main->GetCurrentDescriptorTable();

        material->shader.lock()->SetPipeline(list);

        list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

        DrawRay draw;
        auto cbuffer2 = pool->PopCBuffer("DrawRay", sizeof(DrawRay), 512);
        material->GetData("pos0", draw.linePos[0]);
        material->GetData("pos1", draw.linePos[1]);
        material->GetData("_Color", draw.color);

        cbuffer2.SetData(&draw, sizeof(draw));
        table->SetCurrentGroupHandle(material->shader.lock(), "DrawRay", cbuffer2.handle);


        GraphicManager::main->GetCurrentDescriptorTable()->RecycleCurrentGroupHandle(
            material->shader.lock(), "CameraParams");

        auto a = table->GetCurrentGroupGPUHandle(0);
        list->SetGraphicsRootDescriptorTable(1, a);


        list->DrawInstanced(2, 1, 0, 0);
        //list->DrawInstanced(mesh->vertexCount, 1, 0, 0);

        table->SetNextGroupHandle();
    });
    pack.SetLifeExtension(material);
    SceneManager::GetCurrentScene()->AddRenderPacket(pack);
}

void Draw::Line(const Vector3& start, const Vector3& end, const Color& color)
{
    std::shared_ptr<Material> material = std::make_shared<Material>();
    material->SetShader(ResourceManager::main->GetShader(L"draw_ray"));
    material->SetData("pos0", start);
    material->SetData("pos1", end);
    material->SetData("_Color", Vector4(color));


    RenderPacket pack(nullptr, material, [=](const RenderPacket& render)
        {
            auto material = render.material.lock(); //.lock()
            if (material == nullptr)
                return;

            auto list = GraphicManager::main->GetCurrentCommandList();
            auto pool = GraphicManager::main->GetCurrentCBufferPool();
            auto table = GraphicManager::main->GetCurrentDescriptorTable();

            material->shader.lock()->SetPipeline(list);

            list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

            DrawRay draw;
            auto cbuffer2 = pool->PopCBuffer("DrawRay", sizeof(DrawRay), 512);
            material->GetData("pos0", draw.linePos[0]);
            material->GetData("pos1", draw.linePos[1]);
            material->GetData("_Color", draw.color);

            cbuffer2.SetData(&draw, sizeof(draw));
            table->SetCurrentGroupHandle(material->shader.lock(), "DrawRay", cbuffer2.handle);


            GraphicManager::main->GetCurrentDescriptorTable()->RecycleCurrentGroupHandle(
                material->shader.lock(), "CameraParams");

            auto a = table->GetCurrentGroupGPUHandle(0);
            list->SetGraphicsRootDescriptorTable(1, a);


            list->DrawInstanced(2, 1, 0, 0);
            //list->DrawInstanced(mesh->vertexCount, 1, 0, 0);

            table->SetNextGroupHandle();
        });
    pack.SetLifeExtension(material);
    SceneManager::GetCurrentScene()->AddRenderPacket(pack);
}
