#pragma once
#include <stdafx.h>
#include "Material.h"

class RenderPacket
{
public:
    std::weak_ptr<Material> material;
    std::weak_ptr<Mesh> mesh;
    std::shared_ptr<Material> materialLC = nullptr;
    std::shared_ptr<Mesh> meshLC = nullptr;

    std::function<void(const RenderPacket& renderPack)> renderFunction;
    float zDepth = 0.0f;

    RenderPacket(std::shared_ptr<Mesh> mesh, std::weak_ptr<Material> material, std::function<void(const RenderPacket& renderPack)> renderFunction, float zDepth = 0);
    RenderPacket();
    virtual ~RenderPacket();
    RenderPacket(const RenderPacket& renderPacket);
    RenderPacket(RenderPacket&& renderPacket) noexcept;
    RenderPacket& operator=(const RenderPacket& renderPacket);
    RenderPacket& operator=(RenderPacket&& renderPacket) noexcept;
    std::shared_ptr<RenderPacket> Init(std::shared_ptr<Mesh> mesh, std::weak_ptr<Material> material, std::function<void(const RenderPacket& renderPack)> renderFunction, float zDepth = 0);

    int Order();

    bool operator<(const RenderPacket& rpOther) const;
    bool operator==(const RenderPacket& rpOther) const;

    void SetLifeExtension(std::shared_ptr<Material> material);
    void SetLifeExtension(std::shared_ptr<Mesh> mesh);
};

