#pragma once
#include <stdafx.h>

namespace dxe
{
	class Scene
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
	};
}

