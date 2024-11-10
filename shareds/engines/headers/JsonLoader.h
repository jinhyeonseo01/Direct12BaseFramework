#pragma once
#include <stdafx.h>
#include <EObject.h>
#include <GameObject.h>

using json = nlohmann::json;

namespace dxe
{
    class JsonLoader
    {
    public:
        static json Load(std::wstring path);

        template<class T, std::enable_if_t<std::is_base_of_v<EObject, T>, int> = 0>
        static std::shared_ptr<T> CreateObject(const std::wstring& guid)
        {
            std::shared_ptr<T> object = std::make_shared<T>()->MakeInit<T>();//
            object->SetGUID(guid);
            return object;
        }
    };

    struct GenerateObject
    {
        std::wstring guid;
        std::wstring type;
        std::wstring name;
        std::wstring parent;
        std::vector<std::wstring> components;

        std::wstring componentType;
    };
}

