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
	//���� �ѹ� �����ְ�


	for (int i = gameObjects.size() - 1; i >= 0; --i) // Enable
	{
		std::shared_ptr<GameObject>& currentObject = gameObjects[i];

		//���⼭ �����ȿ�����Ʈ Ȱ��ȭ �ð��� �ʿ��ҵ�.
		if (!currentObject->IsReady())
			currentObject->Ready();

		if ((!currentObject->IsDestroy()) && currentObject->GetActive() && currentObject->IsReady()) {

			//�׷� �� �ܺο��� ���� ���ƾ߰ڳ�
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
	    graphic->_swapChain->Present(1, 0); // vsync�� ����ϸ� �⺻
        /*
         SyncInterval (UINT):

        0: VSync ���� ��� ������ ǥ�� (Ƽ� �߻� ����).
        1: �� ���� VSync ��� �� ������ ǥ�� (�Ϲ����� VSync ���).
        n (n > 1): n���� VSync �ֱ� �� ������ ǥ�� (������ ����Ʈ ����)

        DXGI_PRESENT_NONE (0): �⺻��
        DXGI_PRESENT_ALLOW_TEARING  �񵿱� ���������̼�, ƿ��(Tearing)�� ���, DXGI 1.3
        DXGI_PRESENT_RESTART ��ü ������ �������� �ʱ�ȭ
        DXGI_PRESENT_TEST �������� ������ ǥ������ �ʰ� ���������̼��� �������� �׽�Ʈ
         **/
        graphic->WaitSync();
        graphic->_swapChainIndex = graphic->_swapChain->GetCurrentBackBufferIndex();
	}

}