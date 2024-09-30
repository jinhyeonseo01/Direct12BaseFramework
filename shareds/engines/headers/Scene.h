#pragma once
#include <stdafx.h>

namespace dxe
{
	class Scene
	{
	public:
		std::vector<GameObject> _gameObjectList;

		static void LoadObject(Scene& scene, std::wstring json);
	};
}

