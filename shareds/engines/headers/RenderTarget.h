#pragma once
#include "GraphicManager.h"
#include "stdafx.h"

class RenderTarget : public std::enable_shared_from_this<RenderTarget>
{
public:
    RenderTarget();
    virtual ~RenderTarget();
    
    D3D12_DESCRIPTOR_HEAP_DESC _descHeapDesc;
    ComPtr<ID3D12DescriptorHeap> _descHeap;
    ComPtr<ID3D12Resource2> _recource;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> descHandleList;
    DXGI_FORMAT _format;
    uint32_t GetHandleSize()
    {
        // ÀÛ¾÷Áß
        return GraphicManager::instance->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }
    union
    {
        Vector4 clearColor;
        Vector2 clearDepthStencil;
    };
    union
    {
        D3D12_RENDER_TARGET_VIEW_DESC	    _RTV_ViewDesc;
        D3D12_DEPTH_STENCIL_VIEW_DESC	    _DSV_ViewDesc;
    };
    float* GetColorColor()
    {
        return &clearColor.x;
    }
};

