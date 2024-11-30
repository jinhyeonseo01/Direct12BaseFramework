#pragma once
#include <stdafx.h>
#include "Material.h"

class RenderPacket
{
public:
    std::weak_ptr<Material> material;
    std::weak_ptr<Mesh> mesh;
    std::function<void(const RenderPacket& renderPack)> renderFunction;
    float zDepth;

    RenderPacket(std::shared_ptr<Mesh> mesh, std::weak_ptr<Material> material, std::function<void(const RenderPacket& renderPack)> renderFunction, float zDepth = 0);
    RenderPacket();
    virtual ~RenderPacket();
    int Order();

    bool operator<(const RenderPacket& rpOther) const;
    bool operator==(const RenderPacket& rpOther) const;

    std::shared_ptr<Material> materialLC;
    void SetLifeExtension(std::shared_ptr<Material> material);
};

