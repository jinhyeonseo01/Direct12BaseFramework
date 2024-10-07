#include "DXEngine.h"
#include "Scene.h"
#include "SceneManager.h"

void Engine::LogicPipeline()
{
	std::shared_ptr<Scene>& scene = SceneManager::_currentScene;
	std::shared_ptr<GameObject> currentObject;
	std::vector<std::shared_ptr<GameObject>>& gameObjects = scene->_gameObjectList;
	

	std::stable_sort(gameObjects.begin(), gameObjects.end(), [&](const std::shared_ptr<GameObject>& obj1, const std::shared_ptr<GameObject>& obj2) {
			return obj1->_sortingOrder > obj2->_sortingOrder;
		});
	//정렬 한번 갈겨주고


	for (int i = gameObjects.size() - 1; i >= 0; --i) // Enable
	{
		currentObject = gameObjects[i];
		if ((!currentObject->IsDestroy()) && currentObject->GetActive()) {
			if (currentObject->CheckActiveUpdated()) {
				//currentObject->OnEnable()
				Debug::log << "enable\n";
				currentObject->SetActivePrev(true);
			}
		}
	}

	for (int i = gameObjects.size() - 1; i >= 0; --i) // Disable
	{
		currentObject = gameObjects[i];
		if (currentObject->IsDestroy() || (!currentObject->GetActive())) {
			if (currentObject->CheckActiveUpdated()) {
				//currentObject->OnDisable()
				Debug::log << "disable\n";

				currentObject->SetActivePrev(false);
			}
		}
	}
	for (int i = gameObjects.size() - 1; i >= 0; --i) // OnDestroy
	{
		currentObject = gameObjects[i];
		if (currentObject->IsDestroy()) {
			//OnDestroy
		}
	}
	
	for (int i = gameObjects.size() - 1; i >= 0; --i) // Remover
	{
		currentObject = gameObjects[i];
		if (currentObject->IsDestroy())
			scene->_gameObjectList.erase(scene->_gameObjectList.begin() + i);
	}

}

void Engine::RenderingPipeline()
{
	std::shared_ptr<Scene>& scene = SceneManager::_currentScene;
	std::shared_ptr<GameObject> currentObject;
	std::vector<std::shared_ptr<GameObject>>& gameObjects = scene->_gameObjectList;

	for (int i = gameObjects.size() - 1; i >= 0; --i)
	{
		currentObject = gameObjects[i];
		//GetAcrive -> prevRendering
	}
}