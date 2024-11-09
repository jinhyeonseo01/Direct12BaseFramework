#pragma once
#include <stdafx.h>

using json = nlohmann::json;

namespace dxe
{
    class JsonLoader
    {
    public:
        static json Load(std::wstring path);
    };
}

