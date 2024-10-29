#pragma once
#include "stdafx.h"
#include <memory>

#include "DescriptorTable.h"
#include "Shader.h"
#include "Texture.h"

class Material
{
public:
    std::weak_ptr<Shader> shader;

    std::unordered_map<std::string, int> _propertyInts;
    std::unordered_map<std::string, float> _propertyFloats;
    std::unordered_map<std::string, Vector4> _propertyVectors;
    std::unordered_map<std::string, std::weak_ptr<Texture>> _propertyTextures;

    void SetTextureDatas(std::shared_ptr<DescriptorTable> table, std::shared_ptr<Shader> shader);
};

