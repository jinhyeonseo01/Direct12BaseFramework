#include "stdafx.h"
#include "JsonLoader.h"
#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "EObject.h"
#include "MeshRenderer.h"
#include "Scene.h"
#include "SceneManager.h"


json JsonLoader::Load(std::wstring path)
{
    std::ifstream reader{ path };
    if (!reader)
        return {};

    json readJson;
    reader >> readJson;
    readJson = readJson["references"];

    std::unordered_map<std::string, std::vector<json>> dataTable;

    std::unordered_map<std::wstring, json> refGameObjectTable;
    std::unordered_map<std::wstring, json> refComponentTable;
    std::unordered_map<std::wstring, json> refMaterialTable;

    std::vector<std::shared_ptr<EObject>> objs;

    std::vector<std::wstring> modelList;

    for (auto it = readJson.begin(); it != readJson.end(); ++it)
    {
        auto& key = it.key();
        auto& value = it.value();
        dataTable[key] = {};

        //value.is_array()
        for (auto itObj = value.begin(); itObj != value.end(); ++itObj)
        {
            auto& objJson = itObj.value();
            std::wstring guid = std::to_wstring(objJson["guid"].get<std::string>());

            GenerateObject o;
            if(key == "GameObjects")
            {
                auto object = CreateObject<GameObject>(guid);
                refGameObjectTable[guid] = objJson;
                objs.push_back(object);
            }
            if (key == "Components")
            {
                auto type = std::to_wstring(objJson["type"].get<std::string>());
                if(type == L"Transform") {
                    auto object = CreateObject<Transform>(guid);
                    objs.push_back(object);
                }
                if (type == L"MeshRenderer") {
                    auto object = CreateObject<MeshRenderer>(guid);
                    objs.push_back(object);

                    auto mesh = objJson["mesh"];
                    auto path = std::to_wstring(mesh["path"].get<std::string>());
                    auto modelName = std::to_wstring(mesh["modelName"].get<std::string>());

                    ResourceManager::main->LoadAssimpPack(path, modelName, true);
                    if(std::find(modelList.begin(), modelList.end(), modelName) == modelList.end())
                        modelList.push_back(modelName);
                }
                if (type == L"MeshFilter") {

                }
                refComponentTable[guid] = objJson;
            }
            if(key == "Materials")
            {
                auto material = CreateObject<Material>(guid);
                refMaterialTable[guid] = objJson;
                objs.push_back(material);

                auto datas = objJson["datas"];
                auto textures = datas["textures"];

                for (auto& texture : textures)
                {
                    auto path = std::to_wstring(texture["path"].get<std::string>());
                    auto name = std::to_wstring(texture["originalName"].get<std::string>());
                    ResourceManager::main->LoadTexture(path, name);
                }
                    //material->SetData(textures["name"].get<std::string>(), )
            }
        }
    }
    ResourceManager::main->WaitAll();
    for (auto& modelName : modelList)
        ResourceManager::main->GetModel(modelName)->CreateGraphicResource();

    for (auto& ref : refMaterialTable)
    {
        auto material = EObject::FindObjectByGuid<Material>(ref.first);
        auto datas = ref.second["datas"];
        auto textures = datas["textures"];
        auto floats = datas["floats"];
        auto ints = datas["ints"];
        auto vectors = datas["vectors"];

        for (auto& texture : textures)
            material->SetData(
                texture["name"].get<std::string>(), 
                ResourceManager::main->GetTexture(std::to_wstring(texture["originalName"].get<std::string>())));
        for (auto& data : floats)
            material->SetData(data["name"].get<std::string>(), data["data"].get<float>());
        for (auto& data : ints)
            material->SetData(data["name"].get<std::string>(), data["data"].get<int>());
        for (auto& data : vectors)
            material->SetData(data["name"].get<std::string>(), Vector4(
                data["data"][0].get<float>(),
                data["data"][1].get<float>(),
                data["data"][2].get<float>(),
                data["data"][3].get<float>()
            ));
    }
    for (auto& ref : refComponentTable)
    {
        auto type = std::to_wstring(ref.second["type"].get<std::string>());
        if (type == L"Transform") {
            auto trans = EObject::FindObjectByGuid<Transform>(ref.first);
            Vector3 pos = Vector3(
                ref.second["position"][0].get<float>(),
                ref.second["position"][1].get<float>(),
                ref.second["position"][2].get<float>());
            Quaternion rotation = Quaternion(
                ref.second["rotation"][0].get<float>(),
                ref.second["rotation"][1].get<float>(),
                ref.second["rotation"][2].get<float>(),
                ref.second["rotation"][3].get<float>());
            Vector3 scale = Vector3(
                ref.second["scale"][0].get<float>(),
                ref.second["scale"][1].get<float>(),
                ref.second["scale"][2].get<float>());
            trans->localPosition = pos;
            trans->localRotation = rotation;
            trans->localScale = scale;
        }
        if (type == L"MeshRenderer")
        {
            auto mr = EObject::FindObjectByGuid<MeshRenderer>(ref.first);
            auto meshInfo = ref.second["mesh"];
            auto modelName = std::to_wstring(meshInfo["modelName"].get<std::string>());
            auto model = ResourceManager::main->GetModel(modelName);
            if (model != nullptr)
            {
                auto mesh = model->GetMeshsByName(meshInfo["meshName"].get<std::string>());
                //auto material = std::make_shared<Material>();
                std::vector<std::wstring> materialsGuid;
                for (auto& materialGuid : ref.second["materials"])
                    materialsGuid.push_back(std::to_wstring(materialGuid.get<std::string>()));
                for (auto materialGuid : materialsGuid)
                {
                    auto material = EObject::FindObjectByGuid<Material>(materialGuid);
                    material->shader = ResourceManager::main->GetShader(L"forward");
                    mr->AddMateiral({ material });
                }

                mr->AddMesh(mesh);
                mr->SetModel(model);
                Debug::log << meshInfo["meshName"].get<std::string>() << "\n";
            }
        }
    }
    for (auto& ref : refGameObjectTable)
    {
        auto parentGuid = std::to_wstring(ref.second["parent"].get<std::string>());
        auto name = std::to_wstring(ref.second["name"].get<std::string>());

        auto obj = EObject::FindObjectByGuid<GameObject>(ref.first);
        auto parentObj = EObject::FindObjectByGuid<GameObject>(parentGuid);
        std::vector<std::wstring> componentGuids;
        for (auto& compoGuid : ref.second["components"])
            componentGuids.push_back(std::to_wstring(compoGuid.get<std::string>()));

        if (obj != nullptr)
        {
            for(auto& componentGuid : componentGuids) {
                auto component = EObject::FindObjectByGuid<Component>(componentGuid);
                obj->AddComponent(component);
            }
            obj->name = name;
            obj->Init();
            obj->SetParent(parentObj);
            SceneManager::_currentScene->AddGameObject(obj);
        }

    }

}
