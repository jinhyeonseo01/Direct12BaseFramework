#pragma once

#include <stdafx.h>

#include "EObject.h"
//#include "Scene.h"

namespace dxe
{
    class SceneManager
    {
    public:
        static std::vector<std::shared_ptr<Scene>> _sceneList;
        static std::shared_ptr<Scene> _currentScene;

        template <class T, class = std::enable_if_t<std::is_convertible_v<T*, Scene*>>>
        static std::shared_ptr<T> CreateScene(std::wstring name)
        {
            auto scene = std::make_shared<T>(name)->MakeInit<T>();
            auto sceneParentType = std::dynamic_pointer_cast<Scene>(scene);
            _sceneList.push_back(sceneParentType);
            if (_currentScene == nullptr)
                dxe::SceneManager::ChangeMainScene(sceneParentType);
            scene->Init();
            return scene;
        }

        static std::shared_ptr<Scene> ChangeMainScene(std::wstring name);
        static std::shared_ptr<Scene> ChangeMainScene(std::shared_ptr<Scene> scene);
        static std::shared_ptr<Scene> GetScene(std::wstring name);
        static std::shared_ptr<Scene> GetCurrentScene();
        static std::shared_ptr<Scene> Reset();
        static void DeleteAll();
    };
}
