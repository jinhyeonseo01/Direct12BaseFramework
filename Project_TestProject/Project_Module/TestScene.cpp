#include <stdafx.h>
#include "TestScene.h"
#include <DXEngine.h>
#include "GraphicManager.h"
#include "Camera.h"
#include "CameraController.h"
#include "JsonLoader.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "MeshSample.h"
#include "Scene.h"

void* TestScene::Clone()
{
    return Scene::Clone();
}

void TestScene::ReRef()
{
    Scene::ReRef();
}

TestScene::TestScene()
{
}

TestScene::TestScene(std::wstring name) : Scene(name)
{
}

TestScene::~TestScene()
{
}

void TestScene::Init()
{
    Scene::Init();
    auto rtg = GraphicManager::main->GetRenderTargetGroup(RTGType::SwapChain);

    std::shared_ptr<Shader> shader;

    shader = ResourceManager::main->LoadShader(L"default_forward.hlsl", L"default", rtg->_renderTargetTextureList);
    shader->SetMSAADisable();
    shader->Init();

    shader = ResourceManager::main->LoadShader(L"forward.hlsl", L"forward", rtg->_renderTargetTextureList);
    shader->SetMSAADisable();
    shader->Init();

    shader = ResourceManager::main->LoadShader(L"terrain.hlsl", L"terrain", rtg->_renderTargetTextureList);
    shader->SetMSAADisable();
    shader->Init();

    shader = ResourceManager::main->LoadShader(L"skyBox.hlsl", L"sky", rtg->_renderTargetTextureList);
    shader->SetMSAADisable();
    ShaderInfo info;
    info.cullingType = CullingType::NONE;
    info._zWrite = false;
    info._zTest = false;
    shader->SetShaderSetting(info);
    shader->Init();


    auto cameraObj = CreateGameObject(L"Camera");
    cameraObj->transform->worldPosition(Vector3(0, 0.5, -10.0f));
    cameraObj->AddComponent<Camera>();
    cameraObj->AddComponent<CameraController>();
    camera = cameraObj;

    ResourceManager::main->LoadAssimpPacks({{L"resources/Models/SkyBox.obj", L"SkyBox"}}, true);
    ResourceManager::main->LoadTexture(L"resources/Textures/SkyBoxCubeMap2.png", L"skyTexture", false);
    ResourceManager::main->WaitAll();

    auto skyBox = ResourceManager::main->GetModel(L"SkyBox");
    skyBox->CreateGraphicResource();
    skyMaterial = std::make_shared<Material>();
    skyMaterial->shader = ResourceManager::main->GetShader(L"sky");
    skyMaterial->SetData("skyTexture", ResourceManager::main->GetTexture(L"skyTexture"));


    //JsonLoader::Load(L"resources/scenes/EmaceArt_LavaScene.json", std::dynamic_pointer_cast<Scene>(shared_from_this()));
    JsonLoader::Load(L"resources/scenes/EmaceArt_LavaScene.json", std::dynamic_pointer_cast<Scene>(shared_from_this()));
    //JsonLoader::Load(L"resources/scenes/SimplePoly City - Low Poly Assets_Demo Scene.json", std::dynamic_pointer_cast<Scene>(shared_from_this()));
}

void TestScene::Update()
{
    Scene::Update();
}

void TestScene::RenderingBegin()
{
    Scene::RenderingBegin();

    auto cameraComponent = camera->GetComponent<Camera>();
    auto _aspect = GraphicManager::main->setting.screenInfo.width / GraphicManager::main->setting.screenInfo.height;

    cameraComponent->cameraInfo.ProjectionMatrix = Matrix::CreatePerspectiveFieldOfView(
        cameraComponent->_fovy * D2R, _aspect, cameraComponent->_near, cameraComponent->_far);
    cameraComponent->cameraInfo.ViewMatrix = XMMatrixLookToLH(camera->transform->worldPosition(),
                                                              camera->transform->forward(),
                                                              camera->transform->up());

    auto cameraBuffer = GraphicManager::main->GetCurrentCBufferPool()->PopCBuffer(
        "CameraParams", sizeof(CameraParams), 3);
    cameraBuffer.SetData(&cameraComponent->cameraInfo, sizeof(CameraParams));

    GraphicManager::main->GetCurrentDescriptorTable()->AddRecycleHandle("CameraParams", cameraBuffer.handle);


    /**/
    {
        auto skyBox = ResourceManager::main->GetModel(L"SkyBox");
        auto skyShader = skyMaterial->shader.lock();
        auto skyMesh = skyBox->_meshList[0];


        auto list = GraphicManager::main->GetCurrentCommandList();
        auto pool = GraphicManager::main->GetCurrentCBufferPool();
        auto table = GraphicManager::main->GetCurrentDescriptorTable();

        skyShader->SetPipeline(list);

        list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        list->IASetVertexBuffers(0, 1, &skyMesh->_vertexBufferView);
        list->IASetIndexBuffer(&skyMesh->_indexBufferView);


        auto cbuffer = pool->PopCBuffer("TransformParams");
        TransformParams data;

        data.isSkinned = 0;
        data.WorldMatrix = Matrix::CreateTranslation(camera->transform->worldPosition());
        data.WorldMatrix = Matrix::CreateScale(Vector3(50, 50, 50)) * data.WorldMatrix;
        data.NormalMatrix = data.WorldMatrix.Invert().Transpose();
        cbuffer.SetData(&data, sizeof(data));
        table->SetCurrentGroupHandle(skyShader, "TransformParams", cbuffer.handle);


        auto cbuffer2 = pool->PopCBuffer("DefaultMaterialParams", sizeof(DefaultMaterialParams), 512);
        DefaultMaterialParams data2;
        skyMaterial->GetTextureDatas(table, skyShader);
        skyMaterial->GetData("color", data2.color);

        cbuffer2.SetData(&data2, sizeof(data2));
        table->SetCurrentGroupHandle(skyShader, "DefaultMaterialParams", cbuffer2.handle);

        table->RecycleCurrentGroupHandle(skyShader, "CameraParams");


        auto tableGroupGpuHandle = table->GetCurrentGroupGPUHandle(0);
        list->SetGraphicsRootDescriptorTable(1, tableGroupGpuHandle);

        list->DrawIndexedInstanced(skyMesh->indexCount, 1, 0, 0, 0);
        table->SetNextGroupHandle();
    }
}

void TestScene::RenderingEnd()
{
    Scene::RenderingEnd();
}
