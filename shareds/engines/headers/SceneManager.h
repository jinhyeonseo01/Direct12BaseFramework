#pragma once

#include <stdafx.h>

#include "EObject.h"
//#include "Scene.h"

namespace dxe
{
	class SceneManager : public EObject
	{
	public:

		static std::vector<std::shared_ptr<Scene>> _sceneList;
		static std::shared_ptr<Scene> _currentScene;

		static std::shared_ptr<Scene> CreateScene(std::wstring name);
		static std::shared_ptr<Scene> ChangeMainScene(std::wstring name);
		static std::shared_ptr<Scene> ChangeMainScene(std::shared_ptr<Scene> scene);
		static std::shared_ptr<Scene> GetScene(std::wstring name);

	};
}

