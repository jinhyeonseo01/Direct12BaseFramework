#include <stdafx.h>
#include "Study1Scene.h"
#include <DXEngine.h>
#include "GraphicManager.h"
#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "MeshSample.h"
#include "PlayerComponent.h"
#include "Scene.h"

void* Study1Scene::Clone()
{
    return Scene::Clone();
}

void Study1Scene::ReRef()
{
    Scene::ReRef();
}

Study1Scene::Study1Scene()
{
}

Study1Scene::Study1Scene(std::wstring name) : Scene(name)
{
}

Study1Scene::~Study1Scene()
{
}

std::shared_ptr<Mesh> quad;

void Study1Scene::Init()
{
    Scene::Init();
    auto rtg = GraphicManager::main->GetRenderTargetGroup(RTGType::SwapChain);

    std::shared_ptr<Shader> shader;

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

    shader = ResourceManager::main->LoadShader(L"ui.hlsl", L"ui", rtg->_renderTargetTextureList);
    shader->SetMSAADisable();
    info;
    info.cullingType = CullingType::NONE;
    info._zWrite = true;
    info._zTest = true;
    shader->SetShaderSetting(info);
    shader->Init();


    auto cameraObj = CreateGameObject(L"Camera");
    cameraObj->transform->worldPosition(Vector3(0, 0.5, -10.0f));
    cameraObj->AddComponent<Camera>();
    camera = cameraObj;

    //ResourceManager::main->LoadAssimpPack(L"Ellen.fbx", L"Ellen");
    ResourceManager::main->LoadAssimpPacks({
                                               {L"resources/Models/B.fbx", L"B"},
                                               {L"resources/Models/box.obj", L"box"},
                                               {L"resources/Models/SkyBox.obj", L"SkyBox"},
                                               {L"resources/Models/Apache.fbx", L"Apache"}
                                           }, false);

    ResourceManager::main->LoadTexture(L"resources/Textures/menu.png", L"menu", true);
    ResourceManager::main->LoadTexture(L"resources/Textures/Start.png", L"start", true);
    ResourceManager::main->LoadTexture(L"resources/Textures/help.png", L"help", true);
    ResourceManager::main->LoadTexture(L"resources/Textures/Info.png", L"info", true);

    auto apacheTexture = ResourceManager::main->LoadTexture(L"resources/Textures/Apache_Texture_White.png",
                                                            L"ApacheTexture", true);
    auto apacheTexture2 = ResourceManager::main->LoadTexture(L"resources/Textures/Apache_Texture_Orange.png",
                                                             L"ApacheTexture2", true);
    auto apacheTexture3 = ResourceManager::main->LoadTexture(L"resources/Textures/Apache_Texture_Camo_Green.png",
                                                             L"ApacheTexture3", true);
    auto apacheTexture4 = ResourceManager::main->LoadTexture(L"resources/Textures/Apache_Texture_Purple.png",
                                                             L"ApacheTexture4", true);

    auto bTexture = ResourceManager::main->LoadTexture(L"resources/Textures/GroundStone.png", L"GroundStone", true);
    ResourceManager::main->LoadTexture(L"resources/Textures/Grass01.png", L"Grass01", true);
    ResourceManager::main->LoadTexture(L"resources/Textures/Grass02.png", L"Grass02", true);
    ResourceManager::main->LoadTexture(L"resources/Textures/hm.png", L"hm", true);
    ResourceManager::main->LoadTexture(L"resources/Textures/SkyBoxCubeMap2.png", L"skyTexture", false);

    ResourceManager::main->WaitAll();


    auto skyBox = ResourceManager::main->GetModel(L"SkyBox");
    skyBox->CreateGraphicResource();
    auto apacheModel = ResourceManager::main->GetModel(L"Apache");
    apacheModel->CreateGraphicResource();
    auto bModel = ResourceManager::main->GetModel(L"B");
    bModel->CreateGraphicResource();
    auto boxModel = ResourceManager::main->GetModel(L"box");
    boxModel->CreateGraphicResource();


    quad = MeshSample::CreateQuad();
    quad->CreateBothBuffer();
    auto menu = CreateGameObject(L"menu");
    auto menuMR = menu->AddComponent<MeshRenderer>();
    menuMR->AddMesh({quad});
    auto menuMaterial = std::make_shared<Material>();
    menuMaterial->shader = ResourceManager::main->GetShader(L"ui");
    menuMaterial->SetData("_BaseMap", ResourceManager::main->GetTexture(L"menu"));
    menuMR->AddMateiral({menuMaterial});
    menu->transform->localPosition = Vector3(0, 0, 0.11);
    menu->transform->localScale = Vector3(2, 2, 2);


    menu = CreateGameObject(L"menu_start");
    menuMR = menu->AddComponent<MeshRenderer>();
    menuMR->AddMesh({quad});
    menuMaterial = std::make_shared<Material>();
    menuMaterial->shader = ResourceManager::main->GetShader(L"ui");
    menuMaterial->SetData("_BaseMap", ResourceManager::main->GetTexture(L"start"));
    menuMR->AddMateiral({menuMaterial});
    menu->transform->localPosition = Vector3(0, -0.5, 0.1);
    menu->transform->localScale = Vector3(400 / 1280.0, 150 / 720.0, 2);


    menu = CreateGameObject(L"menu_help");
    menuMR = menu->AddComponent<MeshRenderer>();
    menuMR->AddMesh({quad});
    menuMaterial = std::make_shared<Material>();
    menuMaterial->shader = ResourceManager::main->GetShader(L"ui");
    menuMaterial->SetData("_BaseMap", ResourceManager::main->GetTexture(L"help"));
    menuMR->AddMateiral({menuMaterial});
    menu->transform->localPosition = Vector3(0, -0.8, 0.1);
    menu->transform->localScale = Vector3(400 / 1280.0, 150 / 720.0, 2);


    menu = CreateGameObject(L"menu_info");
    menuMR = menu->AddComponent<MeshRenderer>();
    menuMR->AddMesh({quad});
    menuMaterial = std::make_shared<Material>();
    menuMaterial->shader = ResourceManager::main->GetShader(L"ui");
    menuMaterial->SetData("_BaseMap", ResourceManager::main->GetTexture(L"info"));
    menuMR->AddMateiral({menuMaterial});
    menu->transform->localPosition = Vector3(0, 0, 2);
    menu->transform->localScale = Vector3(1000 * 2 / 1280.0, 600 * 2 / 720.0, 2);


    auto rootObject = CreateGameObject(L"Apache");
    auto apache = CreateGameObjects(apacheModel, apacheModel->rootNode.get());
    apache->transform->localScale = Vector3::One;
    apache->SetParent(rootObject);
    auto player = rootObject->AddComponent<PlayerComponent>();

    std::vector<std::shared_ptr<MeshRenderer>> meshRenderers;
    apache->GetComponentsWithChilds(meshRenderers);
    for (int i = 0; i < meshRenderers.size(); i++)
    {
        auto material = std::make_shared<Material>();
        material->shader = ResourceManager::main->GetShader(L"forward");
        material->SetData("color", Vector4(0, 0, 1, 1));
        material->SetData("_BaseMap", ResourceManager::main->GetTexture(L"ApacheTexture"));
        meshRenderers[i]->AddMateiral({material});
    }

    rootObject = CreateGameObject(L"B2");
    auto b = CreateGameObjects(bModel, bModel->rootNode.get());
    b->transform->localScale = Vector3::One;
    b->transform->localPosition = -Vector3(1, 0, 1) * 50 + Vector3(0, -20, 0);
    b->SetParent(rootObject);

    meshRenderers.clear();
    b->GetComponentsWithChilds(meshRenderers);
    for (int i = 0; i < meshRenderers.size(); i++)
    {
        auto material = std::make_shared<Material>();
        material->shader = ResourceManager::main->GetShader(L"terrain");
        material->SetData("color", Vector4(0, 0, 1, 1));
        material->SetData("_BaseMap", ResourceManager::main->GetTexture(L"Grass01"));
        material->SetData("_BaseMap2", ResourceManager::main->GetTexture(L"Grass02"));
        material->SetData("_BaseMap3", ResourceManager::main->GetTexture(L"GroundStone"));
        meshRenderers[i]->AddMateiral({material});
    }

    srand(11);
    for (int i = 0; i < 5; i++)
    {
        rootObject = CreateGameObject(L"Box_" + i);
        auto box = CreateGameObjects(boxModel);
        box->transform->localScale = Vector3::One;
        box->SetParent(rootObject);
        meshRenderers.clear();
        box->GetComponentsWithChilds(meshRenderers);
        for (int i = 0; i < meshRenderers.size(); i++)
        {
            auto material = std::make_shared<Material>();
            material->shader = ResourceManager::main->GetShader(L"forward");
            material->SetData("color", Vector4(0, 0, 1, 1));
            material->SetData("_BaseMap", ResourceManager::main->GetTexture(L"ApacheTexture"));
            meshRenderers[i]->AddMateiral({material});
            boxMRs.push_back(meshRenderers[i]);
        }
        rootObject->transform->worldPosition(Vector3(-30 + (rand() % 60), 8 + (rand() % 6), -30 + (rand() % 60)));
        rootObject->transform->worldRotation(
            Quaternion::CreateFromYawPitchRoll(Vector3((rand() % 360), (rand() % 360), (rand() % 360)) * D2R));
        auto scale = 1 + (rand() % 40) / 10.0;
        rootObject->transform->localScale = Vector3(scale, scale, scale);
        //boxMRs
    }


    for (int i = 0; i < 7; i++)
    {
        rootObject = CreateGameObject(L"Apache");
        apache = CreateGameObjects(apacheModel, apacheModel->rootNode.get());
        apache->transform->localScale = Vector3::One * 2;
        apache->SetParent(rootObject);
        auto enemy = rootObject->AddComponent<PlayerComponent>();
        enemy->isPlayer = false;
        rootObject->transform->worldPosition(Vector3(-50 + (rand() % 100), 10 + (rand() % 10), -50 + (rand() % 100)));

        meshRenderers.clear();
        apache->GetComponentsWithChilds(meshRenderers);
        for (int i = 0; i < meshRenderers.size(); i++)
        {
            auto material = std::make_shared<Material>();
            material->shader = ResourceManager::main->GetShader(L"forward");
            material->SetData("color", Vector4(0, 0, 1, 1));
            material->SetData("_BaseMap", ResourceManager::main->GetTexture(L"ApacheTexture2"));
            meshRenderers[i]->AddMateiral({material});
        }
    }

    for (auto b : boxMRs)
        player->boxs.push_back(b);

    skyMaterial = std::make_shared<Material>();
    skyMaterial->shader = ResourceManager::main->GetShader(L"sky");
    skyMaterial->SetData("skyTexture", ResourceManager::main->GetTexture(L"skyTexture"));
}

void Study1Scene::Update()
{
    Scene::Update();
}

void Study1Scene::RenderingBegin()
{
    Scene::RenderingBegin();

    auto cameraComponent = camera->GetComponent<Camera>();
    auto _aspect = GraphicManager::main->setting.screenInfo.width / GraphicManager::main->setting.screenInfo.height;

    cameraComponent->cameraInfo.projectionMatrix = Matrix::CreatePerspectiveFieldOfView(
        cameraComponent->_fovy * D2R, _aspect, cameraComponent->_near, cameraComponent->_far);
    cameraComponent->cameraInfo.viewMatrix = XMMatrixLookToLH(camera->transform->worldPosition(),
                                                              camera->transform->forward(),
                                                              camera->transform->up());

    auto cameraBuffer = GraphicManager::main->GetCurrentCBufferPool()->PopCBuffer(
        "CameraParams", sizeof(CameraParams), 3);
    cameraBuffer.SetData(&cameraComponent->cameraInfo, sizeof(CameraParams));

    GraphicManager::main->GetCurrentDescriptorTable()->AddRecycleHandle("CameraParams", cameraBuffer.handle);


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
    //list->DrawInstanced(mesh->vertexCount, 1, 0, 0);

    table->SetNextGroupHandle();
}

void Study1Scene::RenderingEnd()
{
    Scene::RenderingEnd();
}
