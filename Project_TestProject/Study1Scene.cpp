#include <stdafx.h>
#include "Study1Scene.h"
#include <DXEngine.h>
#include "GraphicManager.h"
#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
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

void Study1Scene::Init()
{
    Scene::Init();
    auto rtg = GraphicManager::instance->GetRenderTargetGroup(RTGType::SwapChain);

    std::shared_ptr<Shader> shader;

    shader = ResourceManager::main->LoadShader(L"forward.hlsl", L"forward", rtg->_renderTargetTextureList);
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
    camera = cameraObj;

    //ResourceManager::main->LoadAssimpPack(L"Ellen.fbx", L"Ellen");
    ResourceManager::main->LoadAssimpPack(L"Resources/Models/box.obj", L"box");
    ResourceManager::main->LoadAssimpPack(L"Resources/Models/SkyBox.obj", L"SkyBox");
    ResourceManager::main->LoadAssimpPack(L"Resources/Models/Apache.fbx", L"Apache");
    ResourceManager::main->WaitAll();

    auto skyBox = ResourceManager::main->GetModel(L"SkyBox");
    skyBox->CreateGraphicResource();
    auto apacheModel = ResourceManager::main->GetModel(L"Apache");
    apacheModel->CreateGraphicResource();


    auto rootObject = CreateGameObject(L"Apache");
    auto apache = CreateGameObjects(apacheModel, apacheModel->rootNode.get());
    apache->transform->localScale = Vector3::One * 0.002f;
    apache->SetParent(rootObject);

    auto apacheTexture = ResourceManager::main->LoadTexture(L"Resources/Textures/Apache_Texture_White.png", L"ApacheTexture", true);
    std::vector<std::shared_ptr<MeshRenderer>> meshRenderers;
    apache->GetComponentsWithChilds(meshRenderers);
    for(int i=0;i<meshRenderers.size();i++)
    {
        std::shared_ptr<Material> material = std::make_shared<Material>();
        material->shader = ResourceManager::main->GetShader(L"forward");
        material->SetData("color", Vector4(0, 0, 1, 1));
        material->SetData("_BaseMap", apacheTexture);
        meshRenderers[i]->AddMateiral({ material });
    }

    skyMaterial = std::make_shared<Material>();
    skyMaterial->shader = ResourceManager::main->GetShader(L"sky");
    skyMaterial->SetData("skyTexture", ResourceManager::main->LoadTexture(L"Resources/Textures/SkyBoxCubeMap2.png", L"skyTexture", false));
}

void Study1Scene::Update()
{
    Scene::Update();

}

void Study1Scene::RenderingBegin()
{
    Scene::RenderingBegin();

    auto cameraComponent = camera->GetComponent<Camera>();
    auto _aspect = GraphicManager::instance->setting.screenInfo.width / GraphicManager::instance->setting.screenInfo.height;

    cameraComponent->cameraInfo.projectionMatrix = Matrix::CreatePerspectiveFieldOfView(cameraComponent->_fovy * D2R, _aspect, cameraComponent->_near, cameraComponent->_far);
    cameraComponent->cameraInfo.viewMatrix = XMMatrixLookToLH(camera->transform->worldPosition(),
        camera->transform->forward(),
        camera->transform->up());

    auto cameraBuffer = GraphicManager::instance->GetCurrentCBufferPool()->PopCBuffer("CameraParams", sizeof(CameraParams), 3);
    cameraBuffer.SetData(&cameraComponent->cameraInfo, sizeof(CameraParams));

    GraphicManager::instance->GetCurrentDescriptorTable()->AddRecycleHandle("CameraParams", cameraBuffer.handle);


    auto skyBox = ResourceManager::main->GetModel(L"SkyBox");
    auto skyShader = skyMaterial->shader.lock();
    auto skyMesh = skyBox->_meshList[0];


    auto list = GraphicManager::instance->GetCurrentCommandList();
    auto pool = GraphicManager::instance->GetCurrentCBufferPool();
    auto table = GraphicManager::instance->GetCurrentDescriptorTable();

    skyShader->SetPipeline(list);

    list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    list->IASetVertexBuffers(0, 1, &skyMesh->_vertexBufferView);
    list->IASetIndexBuffer(&skyMesh->_indexBufferView);

    
    auto cbuffer = pool->PopCBuffer("TransformParams");
    TransformParams data;
    
    data.isSkinned = 0;
    data.WorldMatrix = Matrix::CreateTranslation(camera->transform->worldPosition());
    data.WorldMatrix = Matrix::CreateScale(Vector3(50,50,50)) * data.WorldMatrix;
    cbuffer.SetData(&data, sizeof(data));
    table->SetCurrentGroupHandle(skyShader, "TransformParams", cbuffer.handle);
    

    auto cbuffer2 = pool->PopCBuffer("DefaultMaterialParams", sizeof(DefaultMaterialParams), 512);
    DefaultMaterialParams data2;
    skyMaterial->SetTextureDatas(table, skyShader);
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
