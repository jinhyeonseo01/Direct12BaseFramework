#include "DXEngine.h"
#include "Scene.h"
#include "SceneManager.h"

void Engine::LogicPipeline()
{
	std::shared_ptr<Scene>& scene = SceneManager::_currentScene;
	std::vector<std::shared_ptr<GameObject>>& gameObjects = scene->_gameObjectList;
	

	std::stable_sort(gameObjects.begin(), gameObjects.end(), [&](const std::shared_ptr<GameObject>& obj1, const std::shared_ptr<GameObject>& obj2) {
			return obj1->_sortingOrder > obj2->_sortingOrder;
		});
	//정렬 한번 갈겨주고


	for (int i = gameObjects.size() - 1; i >= 0; --i) // Enable
	{
		std::shared_ptr<GameObject>& currentObject = gameObjects[i];

		//여기서 생성된오브젝트 활성화 시간이 필요할듯.
		if (!currentObject->IsReady())
			currentObject->Ready();

		if ((!currentObject->IsDestroy()) && currentObject->GetActive() && currentObject->IsReady()) {

			//그럼 이 외부에서 루프 돌아야겠네
			if (currentObject->CheckActiveUpdated()) { //
				currentObject->SetActivePrev(true);
				auto& components = currentObject->_components;
				for(int j = components.size()-1;j>= 0;--j)
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
					if(components[j]->IsFirst())
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
		if ((!currentObject->IsDestroy()) && currentObject->GetActive() && currentObject->IsReady()) {
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
		if ((!currentObject->IsDestroy()) && currentObject->GetActive() && currentObject->IsReady()) {
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
		if ((!currentObject->IsDestroy()) && currentObject->GetActive() && currentObject->IsReady()) {
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
		if ((currentObject->IsDestroy() || (!currentObject->GetActive())) && currentObject->IsReady()) {
			if (currentObject->CheckActiveUpdated()) {
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
					if(components[j] && components[j]->IsDestroy()) {
						components[j]->OnComponentDestroy();
						components.erase(components.begin() + j);
					}
                    else if(!components[j])
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


	{
        graphic->ResourceBarrier(graphic->_swapChainRT[graphic->_swapChainIndex]->GetResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	    auto list = graphic->GetCommandList();
	    auto allocator = graphic->GetCommandAllocator();
        allocator.Reset();
	    list->Reset(allocator.Get(), nullptr);
	}


	for (int i = gameObjects.size() - 1; i >= 0; --i)
	{
		std::shared_ptr<GameObject>& currentObject = gameObjects[i];

		if ((!currentObject->IsDestroy()) && currentObject->GetActive() && currentObject->IsReady()) {

		}
		//GetAcrive -> prevRendering
	}

	{
        graphic->ResourceBarrier(graphic->_swapChainRT[graphic->_swapChainIndex]->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

        auto currentCommandList = graphic->GetCommandList();
        ID3D12CommandList* commandListArray[] = { currentCommandList.Get() };
        graphic->GetCommandQueue()->ExecuteCommandLists(_countof(commandListArray), commandListArray);
	    graphic->_swapChain->Present(1, 0); // vsync를 사용하며 기본
        /*
         SyncInterval (UINT):

        0: VSync 없이 즉시 프레임 표시 (티어링 발생 가능).
        1: 한 번의 VSync 대기 후 프레임 표시 (일반적인 VSync 사용).
        n (n > 1): n번의 VSync 주기 후 프레임 표시 (프레임 레이트 제한)

        DXGI_PRESENT_NONE (0): 기본값
        DXGI_PRESENT_ALLOW_TEARING  비동기 프레젠테이션, 틸링(Tearing)을 허용, DXGI 1.3
        DXGI_PRESENT_RESTART 전체 프레임 시퀀스를 초기화
        DXGI_PRESENT_TEST 프레임을 실제로 표시하지 않고 프레젠테이션이 가능한지 테스트
         **/
        graphic->WaitSync();
        graphic->_swapChainIndex = graphic->_swapChain->GetCurrentBackBufferIndex();
	}

}