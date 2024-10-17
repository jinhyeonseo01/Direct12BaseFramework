#pragma once
#include <stdafx.h>

#include "Texture.h"

namespace dxe
{

    enum class GroupType
    {
        SwapChain,

    };

    class RenderTargetGroup : public std::enable_shared_from_this<RenderTargetGroup>
    {
    public:
        int order = 0;
        RenderTargetGroup();
        virtual ~RenderTargetGroup();
        std::vector<std::shared_ptr<Texture>> _renderTargetTextureList;
        std::shared_ptr<Texture> _depthStencilTexture;

        ComPtr<ID3D12DescriptorHeap> _renderTargetHeap;
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> _renderTargetHandleList;
        std::vector<int> _renderTargetHandleToTextureIndexList;
        D3D12_CPU_DESCRIPTOR_HANDLE _depthStencilHandle;


        D3D12_VIEWPORT _viewport;
        D3D12_RECT _rect;

        void Create(std::vector<std::shared_ptr<Texture>>& renderTargetList, std::shared_ptr<Texture> depthStencil);

        void ResourceBarrier(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
        void OMSetRenderTargets(uint32_t count, uint32_t offset);
        void OMSetRenderTargets();

        void ClearRenderTargetView(uint32_t index);
        void ClearRenderTargetViews();
        void ClearDepthStencilView();


        void SetViewport(int width, int height);
    };
}
