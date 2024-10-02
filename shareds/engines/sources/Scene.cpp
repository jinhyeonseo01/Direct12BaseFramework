#include "Scene.h"


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


}