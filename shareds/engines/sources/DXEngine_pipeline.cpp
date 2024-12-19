#include "stdafx.h"
#include "DXEngine.h"
#include "Light.h"
#include "RendererComponent.h"
#include "Scene.h"
#include "SceneManager.h"

void Engine::LogicPipeline()
{
    std::shared_ptr<Scene>& scene = SceneManager::_currentScene;
    std::vector<std::shared_ptr<GameObject>>& gameObjects = scene->_gameObjectList;


    std::stable_sort(gameObjects.begin(), gameObjects.end(),
                     [&](const std::shared_ptr<GameObject>& obj1, const std::shared_ptr<GameObject>& obj2)
                     {
                         return obj1->_sortingOrder > obj2->_sortingOrder;
                     });
    //정렬 한번 갈겨주고


    for (int i = gameObjects.size() - 1; i >= 0; --i) // Enable
    {
        std::shared_ptr<GameObject>& currentObject = gameObjects[i];

        //여기서 생성된오브젝트 활성화 시간이 필요할듯.
        if (!currentObject->IsReady())
            currentObject->Ready();

        if ((!currentObject->IsDestroy()) && currentObject->GetActive() && currentObject->IsReady())
        {
            //그럼 이 외부에서 루프 돌아야겠네
            if (currentObject->CheckActiveUpdated())
            {
                //
                currentObject->SetActivePrev(true);
                auto& components = currentObject->_components;
                for (int j = components.size() - 1; j >= 0; --j)
                {
                    components[j]->OnEnable();
                    //enable
                }
            }
            else
            {
                auto& components = currentObject->_components;
                for (int j = components.size() - 1; j >= 0; --j)
                {
                    if (components[j]->IsFirst())
                    {
                        components[j]->OnEnable();
                    }
                }
            }
        }
    }

    for (int i = gameObjects.size() - 1; i >= 0; --i)
    {
        std::shared_ptr<GameObject>& currentObject = gameObjects[i];
        if ((!currentObject->IsDestroy()) && currentObject->GetActive() && currentObject->IsReady())
        {
            //Start
            auto& components = currentObject->_components;
            for (int j = components.size() - 1; j >= 0; --j)
            {
                if (components[j] && components[j]->IsFirst())
                {
                    components[j]->Start();
                    components[j]->FirstDisable();
                }
            }
        }
    }

    for (int i = gameObjects.size() - 1; i >= 0; --i)
    {
        std::shared_ptr<GameObject>& currentObject = gameObjects[i];
        if ((!currentObject->IsDestroy()) && currentObject->GetActive() && currentObject->IsReady())
        {
            auto& components = currentObject->_components;
            for (int j = components.size() - 1; j >= 0; --j)
            {
                if (components[j] && !components[j]->IsFirst())
                    components[j]->Update();
            }
        }
    }
    for (int i = gameObjects.size() - 1; i >= 0; --i)
    {
        std::shared_ptr<GameObject>& currentObject = gameObjects[i];
        if ((!currentObject->IsDestroy()) && currentObject->GetActive() && currentObject->IsReady())
        {
            //LateUpdate
            auto& components = currentObject->_components;
            for (int j = components.size() - 1; j >= 0; --j)
            {
                if (components[j] && !components[j]->IsFirst())
                    components[j]->LateUpdate();
            }
        }
    }

    for (int i = gameObjects.size() - 1; i >= 0; --i) // Disable
    {
        std::shared_ptr<GameObject>& currentObject = gameObjects[i];
        if ((currentObject->IsDestroy() || (!currentObject->GetActive())) && currentObject->IsReady())
        {
            if (currentObject->CheckActiveUpdated())
            {
                currentObject->SetActivePrev(false);
                auto& components = currentObject->_components;
                for (int j = components.size() - 1; j >= 0; --j)
                {
                    if (components[j] && !components[j]->IsFirst())
                        components[j]->OnDisable();
                }
            }
        }
    }
    for (int i = gameObjects.size() - 1; i >= 0; --i) // OnDestroy
    {
        std::shared_ptr<GameObject>& currentObject = gameObjects[i];
        if (currentObject->IsReady())
        {
            if (currentObject->IsDestroy())
            {
                auto& components = currentObject->_components;
                for (int j = components.size() - 1; j >= 0; --j)
                {
                    if (components[j])
                    {
                        components[j]->OnDestroy();
                        components[j]->OnComponentDestroy();
                    }
                }
            }
            else
            {
                auto& components = currentObject->_components;
                for (int j = components.size() - 1; j >= 0; --j)
                {
                    if (components[j] && components[j]->IsDestroy())
                    {
                        components[j]->OnComponentDestroy();
                        components.erase(components.begin() + j);
                    }
                    else if (!components[j])
                        components.erase(components.begin() + j);
                }
            }
        }
    }

    for (int i = gameObjects.size() - 1; i >= 0; --i) // Remover
    {
        std::shared_ptr<GameObject>& currentObject = gameObjects[i];
        if (currentObject->IsDestroy())
            scene->_gameObjectList.erase(scene->_gameObjectList.begin() + i);
    }
}

void Engine::RenderingPipeline()
{
    std::shared_ptr<Scene>& scene = SceneManager::_currentScene;
    std::vector<std::shared_ptr<GameObject>>& gameObjects = scene->_gameObjectList;

    //if (false)
    {
        graphic->RenderPrepare();
        scene->RenderingBegin();
    }

    for (int i = gameObjects.size() - 1; i >= 0; --i)
    {
        std::shared_ptr<GameObject>& currentObject = gameObjects[i];

        if ((!currentObject->IsDestroy()) && currentObject->GetActive() && currentObject->IsReady())
        {
            for (auto& component : currentObject->_components)
            {
                component->BeforeRendering();
            }
        }
        //GetAcrive -> prevRendering
    }


    std::sort(scene->_renderPacketList.begin(), scene->_renderPacketList.end());


    GraphicManager::main->ResourceBarrier(GraphicManager::main->GetRenderTargetGroup(RTGType::Shadow)->_renderTargetTextureList[0]->GetResource(), //renderTargetTextureList[0]
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);//RENDER_TARGET

    GraphicManager::main->GetRenderTargetGroup(RTGType::Shadow)->OMSetRenderTargets();
    GraphicManager::main->GetRenderTargetGroup(RTGType::Shadow)->ClearRenderTargetViews();
    GraphicManager::main->GetRenderTargetGroup(RTGType::Shadow)->ClearDepthStencilView();

    auto lightComponent = Light::GetMainShadowLight();
    auto cameraData = lightComponent->GetCameraParams();

    auto cameraBuffer = GraphicManager::main->GetCurrentCBufferPool()->PopCBuffer(
        "CameraParams", sizeof(CameraParams), 3);
    cameraBuffer.SetData(&cameraData, sizeof(CameraParams));
    GraphicManager::main->GetCurrentDescriptorTable()->AddRecycleHandle("CameraParams", cameraBuffer.handle);

    auto lightBuffer = GraphicManager::main->GetCurrentCBufferPool()->PopCBuffer(
        "MainLightParams", sizeof(MainLightParams), 3);
    lightBuffer.SetData(&cameraData, sizeof(MainLightParams));
    GraphicManager::main->GetCurrentDescriptorTable()->AddRecycleHandle("MainLightParams", lightBuffer.handle);



    for (auto& renderPacket : scene->_renderPacketList)
    {
        if (renderPacket.GetRenderQueueType() == RenderQueueType::Opaque)
        {
            RenderPacket packet = renderPacket;
            packet.material = GraphicManager::main->_shadowMaterial;
            packet.renderFunction(packet);
        }
    }

    auto cameraComponent = Camera::GetMainCamera();//Light::GetMainShadowLight()
    cameraData = cameraComponent->GetCameraParams();
    cameraBuffer = GraphicManager::main->GetCurrentCBufferPool()->PopCBuffer(
        "CameraParams", sizeof(CameraParams), 3);
    cameraBuffer.SetData(&cameraData, sizeof(CameraParams));
    GraphicManager::main->GetCurrentDescriptorTable()->AddRecycleHandle("CameraParams", cameraBuffer.handle);



    GraphicManager::main->ResourceBarrier(GraphicManager::main->GetRenderTargetGroup(RTGType::Shadow)->_renderTargetTextureList[0]->GetResource(),//renderTargetTextureList[0]
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

    GraphicManager::main->GetRenderTargetGroup(RTGType::SwapChain)->OMSetRenderTargets(1, GraphicManager::main->_swapChainIndex);
    GraphicManager::main->GetRenderTargetGroup(RTGType::SwapChain)->ClearRenderTargetView(GraphicManager::main->_swapChainIndex);
    GraphicManager::main->GetRenderTargetGroup(RTGType::SwapChain)->ClearDepthStencilView();

    for (auto& renderPacket : scene->_renderPacketList)
    {
        RenderPacket packet = renderPacket;
        packet.material.lock()->SetData("_MainLightShadowMap", GraphicManager::main->_shadowMap);
        packet.renderFunction(packet);
    }

    
    for (int i = gameObjects.size() - 1; i >= 0; --i)
    {
        std::shared_ptr<GameObject>& currentObject = gameObjects[i];

        if ((!currentObject->IsDestroy()) && currentObject->GetActive() && currentObject->IsReady())
        {
            for (auto& component : currentObject->_components)
            {
                component->AfterRendering();
            }
        }
        //GetAcrive -> prevRendering
    }

    //if(false)
    {
        scene->RenderingEnd();
        graphic->RenderFinish();
    }
}
