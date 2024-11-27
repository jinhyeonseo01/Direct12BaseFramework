#pragma once
#include <stdafx.h>
#include "Material.h"

class RenderPacket
{
public:
    std::weak_ptr<Material> material;
    std::weak_ptr<Component> component;
    std::weak_ptr<Mesh> mesh;
    float zDepth;

    RenderPacket(std::weak_ptr<Mesh> mesh, std::weak_ptr<Material> material, std::weak_ptr<Component> component, float zDepth);

    int Order();

    bool operator<(const RenderPacket& rpOther) const;
    bool operator==(const RenderPacket& rpOther) const;
};

