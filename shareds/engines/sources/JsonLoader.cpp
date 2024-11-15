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
    std::ifstream reader{path};
    if (!reader)
        return {};

    json readJson;
    reader >> readJson;
    readJson = readJson["references"];

    std::unordered_map<std::string, std::vector<json>> dataTable;

    JsonLoader jsonLoader;

    for (auto it = readJson.begin(); it != readJson.end(); ++it)
    {
        auto& key = it.key();
        auto& value = it.value();

        if (key == "GameObjects")
            for (auto& objectJson : value)
                jsonLoader.PrevProcessingGameObject(objectJson);
        if (key == "Components")
            for (auto& objectJson : value)
                jsonLoader.PrevProcessingComponent(objectJson);
        if (key == "Materials")
            for (auto& objectJson : value)
                jsonLoader.PrevProcessingMaterial(objectJson);
    }
    ResourceManager::main->WaitAll();
    for (auto& modelName : jsonLoader.modelNameList)
        if (auto model = ResourceManager::main->GetModel(modelName); model != nullptr)
            model->CreateGraphicResource();

    for (auto& ref : jsonLoader.refMaterialTable)
        jsonLoader.LinkMaterial(ref.second);
    for (auto& ref : jsonLoader.refComponentTable)
        jsonLoader.LinkComponent(ref.second);
    for (auto& ref : jsonLoader.refGameObjectTable)
        jsonLoader.LinkGameObject(ref.second);
}

JsonLoader::JsonLoader()
{
}

JsonLoader::~JsonLoader()
{
}

void JsonLoader::PrevProcessingGameObject(json data)
{
    std::wstring guid = std::to_wstring(data["guid"].get<std::string>());

    auto gameObject = CreateObject<GameObject>(guid)->Init(false);
    refGameObjectTable[guid] = data;
    gameObjectCache.emplace_back(gameObject);
}

void JsonLoader::PrevProcessingComponent(json data)
{
    std::wstring guid = std::to_wstring(data["guid"].get<std::string>());
    std::wstring type = std::to_wstring(data["type"].get<std::string>());
    std::shared_ptr<Component> component;
    refComponentTable[guid] = data;

    if (type == L"Transform")
    {
        auto transform = CreateObject<Transform>(guid);
        component = transform;
    }
    if (type == L"MeshRenderer")
    {
        auto meshRenderer = CreateObject<MeshRenderer>(guid);
        component = meshRenderer;

        auto mesh = data["mesh"];
        auto pack = ResourceManager::main->LoadAssimpPack(
            std::to_wstring(mesh["path"].get<std::string>()),
            std::to_wstring(mesh["modelName"].get<std::string>()),
            true);
        modelNameList.push_back(pack->name);
    }
    if (type == L"MeshFilter")
    {
    }

    componentCache.emplace_back(component);
}

void JsonLoader::PrevProcessingMaterial(json data)
{
    std::wstring guid = std::to_wstring(data["guid"].get<std::string>());
    auto material = CreateObject<Material>(guid);
    refMaterialTable[guid] = data;

    for (auto& texture : data["datas"]["textures"])
    {
        ResourceManager::main->LoadTexture(
            std::to_wstring(texture["path"].get<std::string>()),
            std::to_wstring(texture["originalName"].get<std::string>()));
    }
    materialCache.emplace_back(material);
}

void JsonLoader::LinkGameObject(json jsonData)
{
    std::wstring guid = std::to_wstring(jsonData["guid"].get<std::string>());
    std::wstring parentGuid = std::to_wstring(jsonData["parent"].get<std::string>());

    std::shared_ptr<GameObject> gameObject = EObject::FindObjectByGuid<GameObject>(guid);
    std::shared_ptr<GameObject> parentObject = EObject::FindObjectByGuid<GameObject>(parentGuid);

    if (gameObject != nullptr)
    {
        for (auto& componentGuid : jsonData["components"])
            gameObject->AddComponent(
                EObject::FindObjectByGuid<Component>(std::to_wstring(componentGuid.get<std::string>())));
        gameObject->name = std::to_wstring(jsonData["name"].get<std::string>());
        gameObject->Init();
        gameObject->SetParent(parentObject);
        SceneManager::_currentScene->AddGameObject(gameObject);
    }
}

void JsonLoader::LinkComponent(json jsonData)
{
    std::wstring guid = std::to_wstring(jsonData["guid"].get<std::string>());
    std::wstring type = std::to_wstring(jsonData["type"].get<std::string>());

    if (type == L"Transform")
    {
        auto transform = EObject::FindObjectByGuid<Transform>(guid);
        auto pos = Vector3(
            jsonData["position"][0].get<float>(),
            jsonData["position"][1].get<float>(),
            jsonData["position"][2].get<float>());
        auto rotation = Quaternion(
            jsonData["rotation"][0].get<float>(),
            jsonData["rotation"][1].get<float>(),
            jsonData["rotation"][2].get<float>(),
            jsonData["rotation"][3].get<float>());
        auto scale = Vector3(
            jsonData["scale"][0].get<float>(),
            jsonData["scale"][1].get<float>(),
            jsonData["scale"][2].get<float>());
        transform->localPosition = pos;
        transform->localRotation = rotation;
        transform->localScale = scale;
    }


    if (type == L"MeshRenderer")
    {
        auto meshRenderer = EObject::FindObjectByGuid<MeshRenderer>(guid);
        auto meshInfo = jsonData["mesh"];
        auto modelName = std::to_wstring(meshInfo["modelName"].get<std::string>());

        auto model = ResourceManager::main->GetModel(modelName);
        if (model != nullptr)
        {
            auto mesh = model->GetMeshsByName(meshInfo["meshName"].get<std::string>());

            for (auto& materialGuid : jsonData["materials"])
            {
                auto guid = std::to_wstring(materialGuid.get<std::string>());
                auto material = EObject::FindObjectByGuid<Material>(guid);
                material->shader = ResourceManager::main->GetShader(
                    std::to_wstring(refMaterialTable[guid]["shaderName"].get<std::string>()));
                if (material->shader.lock() == nullptr)
                    Debug::log << "Material ½¦ÀÌ´õ ¸ÅÄª ½ÇÆÐ\n";
                meshRenderer->AddMateiral({material});
            }

            meshRenderer->AddMesh(mesh);
            meshRenderer->SetModel(model);
            //Debug::log << meshInfo["meshName"].get<std::string>() << "\n";
        }
    }
}

void JsonLoader::LinkMaterial(json jsonData)
{
    std::wstring guid = std::to_wstring(jsonData["guid"].get<std::string>());

    auto material = EObject::FindObjectByGuid<Material>(guid);
    auto datas = jsonData["datas"];
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
