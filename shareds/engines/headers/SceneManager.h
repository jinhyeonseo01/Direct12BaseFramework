#pragma once

#include <stdafx.h>
//#include "Scene.h"

namespace dxe
{
	class SceneManager
	{
	public:
		std::vector<std::shared_ptr<Scene>> _sceneList;
		std::shared_ptr<Scene> _currentScene;

		static std::shared_ptr<Scene> CreateScene(std::wstring name);

	};
}

