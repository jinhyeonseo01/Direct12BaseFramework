#pragma once
#include <unordered_map>
#include "GraphicManager.h"
#include "Material.h"

class ResourceManager
{
public:
    std::unordered_map<std::string, std::shared_ptr<Mesh>> meshs;
    std::unordered_map<std::string, std::shared_ptr<Material>> materials;
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;

    ResourceManager();
    virtual ~ResourceManager();
};

