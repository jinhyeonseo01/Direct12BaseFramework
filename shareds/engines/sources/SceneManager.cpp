#include "SceneManager.h"

#include "Scene.h"


std::vector<std::shared_ptr<Scene>> SceneManager::_sceneList;
std::shared_ptr<Scene> SceneManager::_currentScene = nullptr;

//탬플릿으로 빼고 Create<T>로 바꾸고 Init 구조로 잡기
std::shared_ptr<Scene> dxe::SceneManager::CreateScene(std::wstring name)
{
	auto scene = std::make_shared<Scene>(name);
	_sceneList.push_back(scene);
	if (_currentScene == nullptr)
		dxe::SceneManager::ChangeMainScene(scene);
	return scene;
}

std::shared_ptr<Scene> dxe::SceneManager::ChangeMainScene(std::wstring name)
{
	return SceneManager::ChangeMainScene(SceneManager::GetScene(name));
}

std::shared_ptr<Scene> SceneManager::ChangeMainScene(std::shared_ptr<Scene> scene)
{
	std::shared_ptr<Scene> nextScene = scene;
	if (_currentScene != nullptr)
	{
		//todo : Scene 교체시 이벤트 발생시키는 작업 해야함.
	}

	_currentScene = nextScene;
	return _currentScene;
}

std::shared_ptr<Scene> SceneManager::GetScene(std::wstring name)
{
	auto iter = std::find_if(_sceneList.begin(), _sceneList.end(), [&](const std::shared_ptr<Scene>& element) {
		return element->name == name;
		});
	if (iter == _sceneList.end())
		return nullptr;
	return *iter;
}
