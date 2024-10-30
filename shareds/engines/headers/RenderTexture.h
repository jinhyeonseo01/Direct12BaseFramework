#pragma once

#include <stdafx.h>

#include "GraphicSetting.h"

class RenderTexture
{
public:
    RenderTexture();
    virtual ~RenderTexture();


    float* GetClearColor() { return _clearColor; }
    void SetClearColor(Vector4 clearColor)
    {
        _clearColor[0] = clearColor.x;
        _clearColor[1] = clearColor.y;
        _clearColor[2] = clearColor.z;
        _clearColor[3] = clearColor.w;
    };
    void SetSize(Vector2 size)
    {
        _size = size;
    };
    void SetState(ResourceState state);
    void SetFormat(DXGI_FORMAT format)
    {
        this->format = format;
    };

    ComPtr<ID3D12Resource> GetResource() { return _resource; }
    ComPtr<ID3D12DescriptorHeap> GetRTV() { return _RTV_DescHeap; }
    ComPtr<ID3D12DescriptorHeap> GetDSV() { return _DSV_DescHeap; }

    ResourceState					_state = ResourceState::SRV;
    ComPtr<ID3D12Resource>			_resource;
    DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM;

    static std::shared_ptr<RenderTexture> Create(DXGI_FORMAT format, uint32_t width, uint32_t height, const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags, ResourceState state, Vector4 clearColor = Vector4(1, 1, 1, 1));
    static std::shared_ptr<RenderTexture> Link(ComPtr<ID3D12Resource> resource, DXGI_FORMAT format, uint32_t width, uint32_t height, ResourceState state, Vector4
                                               clearColor);
    void CreateFromResource(ComPtr<ID3D12Resource> resource, DXGI_FORMAT format);


    ComPtr<ID3D12DescriptorHeap>	_RTV_DescHeap;
    ComPtr<ID3D12DescriptorHeap>	_DSV_DescHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE		_SRV_CPUHandle{};

    union
    {
        D3D12_RENDER_TARGET_VIEW_DESC	    _RTV_ViewDesc;
        D3D12_DEPTH_STENCIL_VIEW_DESC	    _DSV_ViewDesc;
    };

    float _clearColor[4];
    Vector2 _size = Vector2(0, 0);
};

