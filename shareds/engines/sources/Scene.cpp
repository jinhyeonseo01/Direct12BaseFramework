#include "Scene.h"
#include "Component.h"
#include "Transform.h"
#include "GameObject.h"


namespace dxe
{

	Scene::Scene()
	{

	}

	Scene::Scene(std::wstring name)
	{
		this->SetName(name);
	}

	Scene::Scene(const Scene& scene)
	{

	}

	Scene::Scene(Scene&& scene) noexcept
	{

	}

	Scene& Scene::operator=(const Scene& scene)
	{
		if (this == &scene)
			return *this;
		return *this;
	}

	Scene& Scene::operator=(Scene&& scene) noexcept
	{
		if (this == &scene)
			return *this;
		return *this;
	}

	Scene::~Scene()
	{

	}

	void Scene::SetName(std::wstring name)
	{
		this->name = name;
	}

	void Scene::Reset(std::shared_ptr<Scene> prevScene)
	{
	}

	void Scene::Init()
	{
	}

	void Scene::LoadJsonObject(Scene& scene, std::wstring json)
	{
	}

	std::shared_ptr<GameObject> Scene::CreateGameObject()
	{
		return Scene::CreateGameObject(std::to_wstring(std::format("gameobject({})", 0)));
	}

	std::shared_ptr<GameObject> Scene::CreateGameObject(std::wstring name)
	{
		auto gameObject = std::make_shared<GameObject>(name)->MakeInit<GameObject>();
		gameObject->Init();
		AddGameObject(gameObject);
		return gameObject;
	}

	bool Scene::AddGameObject(std::shared_ptr<GameObject> gameObject)
	{
		if(std::ranges::find(this->_gameObjectList, gameObject) == this->_gameObjectList.end())
		{
			this->_gameObjectList.push_back(gameObject);
			auto scenePtr = std::dynamic_pointer_cast<Scene>(this->shared_from_this());
			gameObject->scene = scenePtr;
			return true;
		}
		return false;
	}

	bool Scene::RemoveGameObject(std::shared_ptr<GameObject> gameObject)
	{
		auto iter = std::find(this->_gameObjectList.begin(), this->_gameObjectList.end(), gameObject);
		if (iter != this->_gameObjectList.end())
		{
			this->_gameObjectList.erase(iter);
			return true;
		}
		return false;

	}

	bool Scene::RemoveAtGameObject(int index)
	{
		if (index >= 0 && index < this->_gameObjectList.size())
		{
			this->_gameObjectList.erase(this->_gameObjectList.begin() + index);
		}
		return false;
	}

	std::shared_ptr<GameObject> Scene::Find(std::wstring name, bool includeDestroy)
	{
		auto iter = std::find_if(this->_gameObjectList.begin(), this->_gameObjectList.end(), [&](const std::shared_ptr<GameObject>& element){
			if (!includeDestroy && element->IsDestroy())
				return false;
			return element->name == name;
		});
		if (iter == this->_gameObjectList.end())
			return nullptr;
		return *iter;
	}

	int Scene::Find(std::wstring name, std::vector<std::shared_ptr<GameObject>> vec, bool includeDestroy)
	{
		if (vec.capacity() < (vec.size() / 2))
			vec.reserve(vec.size());

		for(int i=0;i< this->_gameObjectList.size();i++)
		{
			auto current = this->_gameObjectList[i];
			if (!includeDestroy && current->IsDestroy())
				continue;
			if (current->name == name)
				vec.push_back(current);
		}
		return vec.size();
		
	}

	void Scene::Debug()
	{
		Debug::log << std::format("\nScene : {}", std::to_string(name)) << "\n";
		for(int i=0;i<_gameObjectList.size();i++)
		{
			if(!_gameObjectList[i]->parent.lock())
				_gameObjectList[i]->Debug();
		}
		Debug::log << "\n";
	}
}
