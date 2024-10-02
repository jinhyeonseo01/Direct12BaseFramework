#include "SceneManager.h"

#include "Scene.h"

std::shared_ptr<Scene> dxe::SceneManager::CreateScene(std::wstring name)
{
	return std::make_shared<Scene>();
}
