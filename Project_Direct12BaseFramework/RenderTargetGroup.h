#pragma once
#include <stdafx.h>

#include "Texture.h"

namespace dxe
{
    class RenderTargetGroup : public std::enable_shared_from_this<RenderTargetGroup>
    {
    public:
        RenderTargetGroup();
        virtual ~RenderTargetGroup();
        std::vector<std::shared_ptr<Texture>> _renderTargetList;

        void Create(std::vector<std::shared_ptr<Texture>>& textureGroup, std::shared_ptr<Texture> dsTexture);

        void ResourceBarrier(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
        void OMSetRenderTargets(uint32_t count, uint32_t offset);
        void OMSetRenderTargets();

        void ClearRenderTargetView(uint32_t index);
        void ClearRenderTargetView();
        void ClearDepthStencilView();
    };
}

