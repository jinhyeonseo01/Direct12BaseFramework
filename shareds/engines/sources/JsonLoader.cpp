#include "stdafx.h"
#include "JsonLoader.h"



json JsonLoader::Load(std::wstring path)
{
    std::ifstream reader{ path };
    if (!reader)
        return {};
    json readJson;
    reader >> readJson;
}
