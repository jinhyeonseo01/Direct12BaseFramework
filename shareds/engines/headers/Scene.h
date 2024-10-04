#pragma once
#include <stdafx.h>

#include "EObject.h"

namespace dxe
{
	class Scene : public EObject
	{
	public:
		Scene();
		Scene(std::wstring name);
		Scene(const Scene& scene);
		Scene(Scene&& scene) noexcept;
		Scene& operator=(const Scene& scene);
		Scene& operator=(Scene&& scene) noexcept;
		virtual ~Scene();
	public:
		void SetName(std::wstring name);
		void Reset(std::shared_ptr<Scene> prevScene);
		void Init();
	public:
		std::wstring name;

		std::vector<std::shared_ptr<GameObject>> _gameObjectList;
		std::vector<std::shared_ptr<GameObject>> _destroyObject;

		static void LoadJsonObject(Scene& scene, std::wstring json);
	public:
		std::shared_ptr<GameObject> CreateGameObject();
		std::shared_ptr<GameObject> CreateGameObject(std::wstring name);
		bool AddGameObject(std::shared_ptr<GameObject> gameObject);
		bool RemoveGameObject(std::shared_ptr<GameObject> gameObject);
		bool RemoveAtGameObject(int index);

		std::shared_ptr<GameObject> Find(std::wstring name, bool includeDestroy = false);

	};
}

