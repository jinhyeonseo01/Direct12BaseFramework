#include <stdafx.h>
#include "Study1Scene.h"
#include <DXEngine.h>
#include "GraphicManager.h"
#include "Camera.h"
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
    shader->Init();


    auto cameraObj = CreateGameObject(L"Camera");
    cameraObj->transform->worldPosition(Vector3(0, 0.5, -10.0f));
    cameraObj->AddComponent<Camera>();


    //ResourceManager::main->LoadAssimpPack(L"Ellen.fbx", L"Ellen");
    ResourceManager::main->LoadAssimpPack(L"models/box.obj", L"box");
    ResourceManager::main->LoadAssimpPack(L"models/SkyBox.obj", L"SkyBox");
    ResourceManager::main->LoadAssimpPack(L"Kind.fbx", L"Kind");
    ResourceManager::main->WaitAll();

    auto skyBox = ResourceManager::main->GetModel(L"SkyBox");
    skyBox->CreateGraphicResource();
    auto model = ResourceManager::main->GetModel(L"Kind");
    model->CreateGraphicResource();

    auto obj = CreateGameObjects(model, model->rootNode.get());
    
    
}

void Study1Scene::Update()
{
    Scene::Update();

}

void Study1Scene::RenderingBegin()
{
    Scene::RenderingBegin();
    auto skyBox = ResourceManager::main->GetModel(L"SkyBox");
    
}

void Study1Scene::RenderingEnd()
{
    Scene::RenderingEnd();

}
