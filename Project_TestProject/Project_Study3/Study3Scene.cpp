#include <stdafx.h>
#include "Study3Scene.h"
#include <DXEngine.h>
#include "GraphicManager.h"
#include "Camera.h"
#include "JsonLoader.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "MeshSample.h"
#include "Scene.h"
#include "CBuffer_struct.h"
#include "Light.h"
#include "../Project_Module/CameraController.h"
#include "../Project_Study1/PlayerComponent.h"
#include "../Project_Study2/TreeRenderer.h"

void* Study3Scene::Clone()
{
    return Scene::Clone();
}

void Study3Scene::ReRef()
{
    Scene::ReRef();
}

Study3Scene::Study3Scene()
{
}

Study3Scene::Study3Scene(std::wstring name) : Scene(name)
{
}

Study3Scene::~Study3Scene()
{
}

void Study3Scene::Init()
{
    Scene::Init();
    auto rtg = GraphicManager::main->GetRenderTargetGroup(RTGType::SwapChain);

    std::shared_ptr<Shader> shader;


    ShaderInfo info;

    shader = ResourceManager::main->LoadShader(L"forward.hlsl", L"forward", rtg->_renderTargetTextureList);
    shader->SetMSAADisable();
    shader->Init();

    shader = ResourceManager::main->LoadShader(L"terrain.hlsl", L"terrain", rtg->_renderTargetTextureList);
    info._renderQueueType = RenderQueueType::Opaque;
    shader->SetMSAADisable();
    shader->Init();

    shader = ResourceManager::main->LoadShader(L"default_forward.hlsl", L"wireframe", rtg->_renderTargetTextureList);
    shader->SetMSAADisable();
    info.cullingType = CullingType::WIREFRAME;
    info._zWrite = true;
    info._zTest = true;
    shader->SetShaderSetting(info);
    shader->Init();

    shader = ResourceManager::main->LoadShaderEx(L"forward_tess.hlsl", L"forward_tess", {
    {"VS_Main", "vs"},
    {"PS_Main", "ps"},
    { "HS_Main", "hs" },
    { "DS_Main", "ds" }
    },
    GraphicManager::main->setting.shaderMacro, rtg->_renderTargetTextureList);
    ShaderInfo info2;
    info2._renderQueueType = RenderQueueType::Opaque;
    info2.cullingType = CullingType::WIREFRAME;
    info2._zWrite = true;
    info2._zTest = true;
    info2._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    shader->SetShaderSetting(info2);
    shader->SetMSAADisable();
    shader->Init();


    shader = ResourceManager::main->LoadShader(L"skyBox.hlsl", L"sky", rtg->_renderTargetTextureList);
    shader->SetMSAADisable();
    info = ShaderInfo();
    info.cullingType = CullingType::NONE;
    info._renderQueueType = RenderQueueType::Sky;
    info._zWrite = false;
    info._zTest = false;
    shader->SetShaderSetting(info);
    shader->Init();

    shader = ResourceManager::main->LoadShader(L"mirror.hlsl", L"mirror", rtg->_renderTargetTextureList);
    shader->SetMSAADisable();
    info = ShaderInfo();
    info.cullingType = CullingType::NONE;
    info._renderQueueType = RenderQueueType::Geometry;
    info._zWrite = true;
    info._zTest = true;
    info._stencilIndex = 1;
    info._stencilTest = true;
    info._stencilComp = CompOper::Always;
    info._stencilFailOp = D3D12_STENCIL_OP_KEEP;
    info._stencilPassOp = D3D12_STENCIL_OP_REPLACE;
    info._stencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

    shader->SetShaderSetting(info);
    shader->Init();


    shader = ResourceManager::main->LoadShader(L"mirror2.hlsl", L"mirror2", rtg->_renderTargetTextureList);
    shader->SetMSAADisable();
    info = ShaderInfo();
    info.cullingType = CullingType::NONE;
    info._renderQueueType = RenderQueueType::Geometry;
    info._zWrite = true;
    info._zTest = true;
    info._zComp = CompOper::Always;
    info._stencilIndex = 1;
    info._stencilTest = true;
    info._stencilComp = CompOper::Equal;
    info._stencilFailOp = D3D12_STENCIL_OP_KEEP;
    info._stencilPassOp = D3D12_STENCIL_OP_KEEP;
    info._stencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    shader->SetShaderSetting(info);
    shader->Init();

    shader = ResourceManager::main->LoadShader(L"forward2.hlsl", L"forward2", rtg->_renderTargetTextureList);
    shader->SetMSAADisable();
    info = ShaderInfo();
    info.cullingType = CullingType::FRONT;
    info._renderQueueType = RenderQueueType::Opaque;
    info._zWrite = true;
    info._zTest = true;
    info._stencilIndex = 1;
    info._stencilTest = true;
    info._stencilComp = CompOper::Equal;
    info._stencilFailOp = D3D12_STENCIL_OP_KEEP;
    info._stencilPassOp = D3D12_STENCIL_OP_KEEP;
    info._stencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    shader->SetShaderSetting(info);
    shader->Init();


    shader = ResourceManager::main->LoadShader(L"ui.hlsl", L"ui", rtg->_renderTargetTextureList);
    shader->SetMSAADisable();
    info = ShaderInfo();
    info.cullingType = CullingType::NONE;
    info._renderQueueType = RenderQueueType::UI;
    info._zWrite = true;
    info._zTest = true;
    shader->SetShaderSetting(info);
    shader->Init();



    shader = ResourceManager::main->LoadShaderEx(L"tree.hlsl", L"tree", {
        {"GS_Main", "gs"},
        {"VS_Main", "vs"},
        {"PS_Main", "ps"} },
        GraphicManager::main->setting.shaderMacro,
        rtg->_renderTargetTextureList);
    shader->SetMSAADisable();
    info.cullingType = CullingType::BACK;
    info._zWrite = true;
    info._zTest = true;
    info._renderQueueType = RenderQueueType::AlphaTest;
    info._blendType[0] = BlendType::AlphaBlend;
    info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    shader->SetShaderSetting(info);
    shader->Init();


    shader = ResourceManager::main->LoadShaderEx(L"gress.hlsl", L"gress", {
    {"GS_Main", "gs"},
    {"VS_Main", "vs"},
    {"PS_Main", "ps"} },
    GraphicManager::main->setting.shaderMacro,
    rtg->_renderTargetTextureList);
    shader->SetMSAADisable();
    info.cullingType = CullingType::BACK;
    info._zWrite = true;
    info._zTest = true;
    info._renderQueueType = RenderQueueType::Opaque;
    info._blendType[0] = BlendType::AlphaBlend;
    info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    shader->SetShaderSetting(info);
    shader->Init();

    shader = ResourceManager::main->LoadShaderEx(L"water.hlsl", L"water", {
    {"VS_Main", "vs"},
    {"PS_Main", "ps"} },
    GraphicManager::main->setting.shaderMacro,
    rtg->_renderTargetTextureList);
    shader->SetMSAADisable();
    info.cullingType = CullingType::NONE;
    info._zWrite = false;
    info._zTest = true;
    info._renderQueueType = RenderQueueType::Transparent;
    info._blendType[0] = BlendType::AlphaBlend;
    info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    shader->SetShaderSetting(info);
    shader->Init();



    shader = ResourceManager::main->LoadShader(L"default_forward.hlsl", L"wireframe", rtg->_renderTargetTextureList);
    shader->SetMSAADisable();
    info.cullingType = CullingType::WIREFRAME;
    info._zWrite = true;
    info._zTest = true;
    shader->SetShaderSetting(info);
    shader->Init();

    shader = ResourceManager::main->LoadShader(L"draw_ray.hlsl", L"draw_ray", rtg->_renderTargetTextureList);
    shader->SetMSAADisable();
    info.cullingType = CullingType::NONE;
    info._zWrite = true;
    info._zTest = true;
    info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    shader->SetShaderSetting(info);
    shader->Init();


    auto cameraObj = CreateGameObject(L"Camera");
    cameraObj->transform->worldPosition(Vector3(0, 0.5, -10.0f));
    auto cameraComponent = cameraObj->AddComponent<Camera>();
    cameraComponent->SetMainCamera();
    camera = cameraObj;


    //ResourceManager::main->LoadAssimpPack(L"Ellen.fbx", L"Ellen");
    ResourceManager::main->LoadAssimpPacks({
        {L"resources/Models/B.fbx",L"B"},
        {L"resources/Models/box.obj",L"box"},
        {L"resources/Models/SkyBox.obj",L"SkyBox"},
        {L"resources/Models/Apache.fbx", L"Apache"},
        {L"resources/Models/Plane.fbx", L"Plane" }
        }, false);

    ResourceManager::main->LoadTexture(L"resources/Textures/menu.png", L"menu", true);
    ResourceManager::main->LoadTexture(L"resources/Textures/Start.png", L"start", true);
    ResourceManager::main->LoadTexture(L"resources/Textures/help.png", L"help", true);
    ResourceManager::main->LoadTexture(L"resources/Textures/Info.png", L"info", true);
    ResourceManager::main->LoadTexture(L"resources/Textures/tree.png", L"tree", true);
    ResourceManager::main->LoadTexture(L"resources/Textures/water.png", L"Water", true);

    auto apacheTexture = ResourceManager::main->LoadTexture(L"resources/Textures/Apache_Texture_White.png", L"ApacheTexture", true);
    auto apacheTexture2 = ResourceManager::main->LoadTexture(L"resources/Textures/Apache_Texture_Orange.png", L"ApacheTexture2", true);
    auto apacheTexture3 = ResourceManager::main->LoadTexture(L"resources/Textures/Apache_Texture_Camo_Green.png", L"ApacheTexture3", true);
    auto apacheTexture4 = ResourceManager::main->LoadTexture(L"resources/Textures/Apache_Texture_Purple.png", L"ApacheTexture4", true);

    auto bTexture = ResourceManager::main->LoadTexture(L"resources/Textures/GroundStone.png", L"GroundStone", true);
    ResourceManager::main->LoadTexture(L"resources/Textures/Grass01.png", L"Grass01", true);
    ResourceManager::main->LoadTexture(L"resources/Textures/Grass02.png", L"Grass02", true);
    ResourceManager::main->LoadTexture(L"resources/Textures/hm.png", L"hm", true);
    ResourceManager::main->LoadTexture(L"resources/Textures/SkyBoxCubeMap2.png", L"skyTexture", false);

    ResourceManager::main->WaitAll();

    auto skyBox = ResourceManager::main->GetModel(L"SkyBox");
    skyBox->CreateGraphicResource();
    auto bModel = ResourceManager::main->GetModel(L"B");
    bModel->CreateGraphicResource();
    auto boxModel = ResourceManager::main->GetModel(L"box");
    boxModel->CreateGraphicResource();
    auto waterModel = ResourceManager::main->GetModel(L"Plane");
    waterModel->CreateGraphicResource();

    JsonLoader::Load(L"resources/scenes/Study3 Scene.json", std::dynamic_pointer_cast<Scene>(shared_from_this()));

    auto apacheModel = ResourceManager::main->GetModel(L"Apache");




    quad = MeshSample::CreateQuad();
    quad->CreateBothBuffer();


    auto menu = CreateGameObject(L"menu");
    auto menuMR = menu->AddComponent<MeshRenderer>();
    menuMR->AddMesh({ quad });
    auto menuMaterial = std::make_shared<Material>();
    menuMaterial->shader = ResourceManager::main->GetShader(L"ui");
    menuMaterial->SetData("_BaseMap", ResourceManager::main->GetTexture(L"menu"));
    //menuMaterial->SetData("_BaseMap", GraphicManager::main->_shadowMap);
    menuMR->AddMateiral({ menuMaterial });
    menu->transform->localPosition = Vector3(0, 0, 0.1105);
    menu->transform->localScale = Vector3(2, 2, 2);


    menu = CreateGameObject(L"menu_start");
    menuMR = menu->AddComponent<MeshRenderer>();
    menuMR->AddMesh({ quad });
    menuMaterial = std::make_shared<Material>();
    menuMaterial->shader = ResourceManager::main->GetShader(L"ui");
    menuMaterial->SetData("_BaseMap", ResourceManager::main->GetTexture(L"start"));
    menuMR->AddMateiral({ menuMaterial });
    menu->transform->localPosition = Vector3(0, -0.5, 0.1101);
    menu->transform->localScale = Vector3(400 / 1280.0, 150 / 720.0, 2);


    menu = CreateGameObject(L"menu_help");
    menuMR = menu->AddComponent<MeshRenderer>();
    menuMR->AddMesh({ quad });
    menuMaterial = std::make_shared<Material>();
    menuMaterial->shader = ResourceManager::main->GetShader(L"ui");
    menuMaterial->SetData("_BaseMap", ResourceManager::main->GetTexture(L"help"));
    menuMR->AddMateiral({ menuMaterial });
    menu->transform->localPosition = Vector3(0, -0.8, 0.1102);
    menu->transform->localScale = Vector3(400 / 1280.0, 150 / 720.0, 2);


    menu = CreateGameObject(L"menu_info");
    menuMR = menu->AddComponent<MeshRenderer>();
    menuMR->AddMesh({ quad });
    menuMaterial = std::make_shared<Material>();
    menuMaterial->shader = ResourceManager::main->GetShader(L"ui");
    menuMaterial->SetData("_BaseMap", ResourceManager::main->GetTexture(L"info"));
    menuMR->AddMateiral({ menuMaterial });
    menu->transform->localPosition = Vector3(0, 0, 2);
    menu->transform->localScale = Vector3(1000 * 2 / 1280.0, 600 * 2 / 720.0, 2);


    std::vector<std::shared_ptr<MeshRenderer>> meshRenderers;

    auto apache = SceneManager::GetCurrentScene()->Find(L"Apache");
    auto player = apache->AddComponent<PlayerComponent>();
    /*
    apache->GetComponentsWithChilds(meshRenderers);
    for (int i = 0; i < meshRenderers.size(); i++)
    {
        std::shared_ptr<Material> material = std::make_shared<Material>();
        material->shader = ResourceManager::main->GetShader(L"forward");
        material->SetData("color", Vector4(0, 0, 1, 1));
        material->SetData("_BaseMap", ResourceManager::main->GetTexture(L"ApacheTexture"));
        meshRenderers[i]->AddMateiral({ material });
    }
    */

    auto rootObject = CreateGameObject(L"B2");
    auto b = CreateGameObjects(bModel, bModel->rootNode.get());
    b->transform->localScale = Vector3::One * 10;
    b->transform->localPosition = (-Vector3(1, 0, 1) + Vector3(0, -0.3, 0)) * 50 * 10;
    b->SetParent(rootObject);

    
    auto tree = CreateGameObject(L"Trees");
    auto treeRenderer = tree->AddComponent<TreeRenderer>();
    auto treeMaterial = std::make_shared<Material>();
    treeMaterial->shader = ResourceManager::main->GetShader(L"tree");
    treeMaterial->SetData("_BaseMap", ResourceManager::main->GetTexture(L"tree"));
    treeRenderer->AddMateiral({ treeMaterial });

    /*
    auto gress = CreateGameObject(L"Gress");
    auto gressRenderer = tree->AddComponent<TreeRenderer>();
    auto gressMaterial = std::make_shared<Material>();
    gressMaterial->shader = ResourceManager::main->GetShader(L"gress");
    gressRenderer->AddMateiral({ gressMaterial });
    gressRenderer->count = 1000000;
    */


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
        meshRenderers[i]->AddMateiral({ material });
    }

    srand(11);
    for (int i = 0; i < 5; i++)
    {
        rootObject = CreateGameObject(L"Box_" + std::to_wstring(i));
        auto box = CreateGameObjects(boxModel);
        box->transform->localScale = Vector3::One;
        box->SetParent(rootObject);
        meshRenderers.clear();
        box->GetComponentsWithChilds(meshRenderers);
        for (int i = 0; i < meshRenderers.size(); i++)
        {
            std::shared_ptr<Material> material = std::make_shared<Material>();
            material->shader = ResourceManager::main->GetShader(L"forward");
            material->SetData("color", Vector4(0, 0, 1, 1));
            material->SetData("_BaseMap", ResourceManager::main->GetTexture(L"ApacheTexture"));
            meshRenderers[i]->AddMateiral({ material });
            boxMRs.push_back(meshRenderers[i]);
        }
        rootObject->transform->worldPosition(Vector3(-30 + (rand() % 60), 8 + (rand() % 6), -30 + (rand() % 60)));
        rootObject->transform->worldRotation(Quaternion::CreateFromYawPitchRoll(Vector3((rand() % 360), (rand() % 360), (rand() % 360)) * D2R));
        auto scale = 1 + (rand() % 40) / 10.0;
        rootObject->transform->localScale = Vector3(scale, scale, scale);
        //boxMRs
    }


    for (int i = 0; i < 7; i++)
    {
        rootObject = CreateGameObject(L"Apache");
        apache = CreateGameObjects(apacheModel, apacheModel->rootNode.get());
        //apache->transform->localScale = Vector3::One * 0.004f;
        apache->SetParent(rootObject);
        auto enemy = rootObject->AddComponent<PlayerComponent>();
        enemy->isPlayer = false;
        rootObject->transform->worldPosition(Vector3(-50 + (rand() % 100), 10 + (rand() % 10), -50 + (rand() % 100)));

        meshRenderers.clear();
        apache->GetComponentsWithChilds(meshRenderers);
        for (int i = 0; i < meshRenderers.size(); i++)
        {
            std::shared_ptr<Material> material = std::make_shared<Material>();
            material->shader = ResourceManager::main->GetShader(L"forward");
            material->SetData("color", Vector4(0, 0, 1, 1));
            material->SetData("_BaseMap", ResourceManager::main->GetTexture(L"ApacheTexture2"));
            meshRenderers[i]->AddMateiral({ material });
        }

    }



    rootObject = CreateGameObject(L"Water");
    auto water = CreateGameObjects(waterModel);
    water->transform->localScale = Vector3::One;
    water->SetParent(rootObject);
    meshRenderers.clear();
    water->GetComponentsWithChilds(meshRenderers);
    for (int i = 0; i < meshRenderers.size(); i++)
    {
        std::shared_ptr<Material> material = std::make_shared<Material>();
        material->shader = ResourceManager::main->GetShader(L"water");
        material->SetData("color", Vector4(0, 0, 1, 0.1f));
        material->SetData("_BaseMap", ResourceManager::main->GetTexture(L"Water"));
        meshRenderers[i]->AddMateiral({ material });
    }
    rootObject->transform->worldPosition(Vector3(0, -120, 0));
    rootObject->transform->localScale = Vector3(100, 100, 100);



    for (auto b : boxMRs)
        player->boxs.push_back(b);






    auto light = CreateGameObject(L"Light");
    auto lightComponent = light->AddComponent<Light>();
    light->transform->localPosition = Vector3(0, 100, 0);
    light->transform->localRotation = Quaternion::CreateFromYawPitchRoll(Vector3(65, 45, 0) * D2R);
    lightComponent->orthoSize = Vector2(300, 300);
    lightComponent->_far = 500;
    

    rootObject = CreateGameObject(L"BoxTess");
    auto box = CreateGameObjects(boxModel);
    box->transform->localScale = Vector3::One;
    box->SetParent(rootObject);
    meshRenderers.clear();
    box->GetComponentsWithChilds(meshRenderers);
    for (int i = 0; i < meshRenderers.size(); i++)
    {
        std::shared_ptr<Material> material = std::make_shared<Material>();
        material->shader = ResourceManager::main->GetShader(L"forward_tess");
        material->SetData("color", Vector4(0, 0, 1, 1));
        material->SetData("_BaseMap", ResourceManager::main->GetTexture(L"noneTexture"));
        meshRenderers[i]->AddMateiral({ material });
        meshRenderers[i]->isTess = true;
        boxMRs.push_back(meshRenderers[i]);
    }
    rootObject->transform->worldPosition(Vector3(0,-20,0));
    rootObject->transform->localScale = Vector3(30, 1, 30);



    skyMaterial = std::make_shared<Material>();
    skyMaterial->shader = ResourceManager::main->GetShader(L"sky");
    skyMaterial->SetData("skyTexture", ResourceManager::main->GetTexture(L"skyTexture"));



    auto mirror = Find(L"Mirror");
}

void Study3Scene::Update()
{
    Scene::Update();
}

void Study3Scene::RenderingBegin()
{
    Scene::RenderingBegin();


    auto cameraComponent = camera->GetComponent<Camera>();
    auto cameraData = cameraComponent->GetCameraParams();
    _cameraParams = cameraData;
    auto cameraBuffer = GraphicManager::main->GetCurrentCBufferPool()->PopCBuffer(
        "CameraParams", sizeof(CameraParams), 3);
    cameraBuffer.SetData(&cameraData, sizeof(CameraParams));

    GraphicManager::main->GetCurrentDescriptorTable()->AddRecycleHandle("CameraParams", cameraBuffer.handle);


    auto skyBox = ResourceManager::main->GetModel(L"SkyBox");
    auto skyShader = skyMaterial->shader.lock();
    auto skyMesh = skyBox->_meshList[0];


    auto list = GraphicManager::main->GetCurrentCommandList();
    auto pool = GraphicManager::main->GetCurrentCBufferPool();
    auto table = GraphicManager::main->GetCurrentDescriptorTable();

    RenderPacket pack(nullptr, skyMaterial, [=](const RenderPacket& packet) {

        float skyBoxSize = 50;


        skyShader->SetPipeline(list);

        list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        list->IASetVertexBuffers(0, 1, &skyMesh->_vertexBufferView);
        list->IASetIndexBuffer(&skyMesh->_indexBufferView);


        auto cbuffer = pool->PopCBuffer("TransformParams");
        TransformParams data;

        data.isSkinned = 0;
        data.WorldMatrix = Matrix::CreateTranslation(camera->transform->worldPosition());
        data.WorldMatrix = Matrix::CreateScale(skyBoxSize) * data.WorldMatrix;
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
    });
    SceneManager::GetCurrentScene()->AddRenderPacket(pack);
}

void Study3Scene::RenderingEnd()
{
    Scene::RenderingEnd();
}
