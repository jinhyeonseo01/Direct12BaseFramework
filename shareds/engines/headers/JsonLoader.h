#pragma once
#include <stdafx.h>
#include <EObject.h>
#include <GameObject.h>

#include "Material.h"

using json = nlohmann::json;

namespace dxe
{
    class JsonLoader
    {
    public:
        static json Load(std::wstring path, std::shared_ptr<Scene> scene);

        template <class T, std::enable_if_t<std::is_base_of_v<EObject, T>, int>  = 0>
        static std::shared_ptr<T> CreateObject(const std::wstring& guid)
        {
            std::shared_ptr<T> object = std::make_shared<T>()->MakeInit<T>(); //
            object->SetGUID(guid);
            return object;
        }

        JsonLoader();
        virtual ~JsonLoader();

        std::unordered_map<std::wstring, json> refGameObjectTable;
        std::unordered_map<std::wstring, json> refComponentTable;
        std::unordered_map<std::wstring, json> refMaterialTable;

        std::vector<std::shared_ptr<Component>> componentCache;
        std::vector<std::shared_ptr<GameObject>> gameObjectCache;
        std::vector<std::shared_ptr<Material>> materialCache;

        std::vector<std::wstring> modelNameList;

        std::weak_ptr<Scene> scene;

        void PrevProcessingGameObject(json data);
        void PrevProcessingComponent(json data);
        void PrevProcessingMaterial(json data);

        void LinkGameObject(json jsonData);
        void LinkComponent(json jsonData);
        void LinkMaterial(json jsonData);
    };
}
