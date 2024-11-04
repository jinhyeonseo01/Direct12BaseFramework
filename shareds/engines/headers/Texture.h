#pragma once
#include <stdafx.h>
#include <string>

#include "GraphicSetting.h"
#include "RenderTexture.h"

class Texture : public std::enable_shared_from_this<Texture>
{
public:
    Texture();
    virtual ~Texture();
    ComPtr<ID3D12Resource> GetResource() { return _resource; }
    ComPtr<ID3D12DescriptorHeap> GetRTV() { return _RTV_DescHeap; }
    ComPtr<ID3D12DescriptorHeap> GetDSV() { return _DSV_DescHeap; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() { return _SRV_CPUHandle; }
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

    ResourceState					_state = ResourceState::SRV;
    ScratchImage			 		_image;
    ComPtr<ID3D12Resource>			_resource;

    static std::shared_ptr<Texture> Create(DXGI_FORMAT format, uint32_t width, uint32_t height, const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags, Vector4 clearColor = Vector4(1,1,1,1));
    static std::shared_ptr<Texture> Load(const std::wstring& path, bool createMipMap = false);

    static std::shared_ptr<Texture> Link(std::shared_ptr<RenderTexture> renderTexture, DXGI_FORMAT format);
    static std::shared_ptr<Texture> Link(ComPtr<ID3D12Resource> resource, DXGI_FORMAT format, uint32_t width, uint32_t height, int mipLevels = 1);

    Vector4 GetPixel(int x, int y, int channal = 4) const;

    void CreateFromResource(ComPtr<ID3D12Resource> resource, DXGI_FORMAT format);


    ComPtr<ID3D12DescriptorHeap>	_RTV_DescHeap;
    ComPtr<ID3D12DescriptorHeap>	_DSV_DescHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE		_SRV_CPUHandle{};

    union
    {
        D3D12_RENDER_TARGET_VIEW_DESC	    _RTV_ViewDesc;
        D3D12_DEPTH_STENCIL_VIEW_DESC	    _DSV_ViewDesc;
    };

    union
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC	    _SRV_ViewDesc;
        D3D12_CONSTANT_BUFFER_VIEW_DESC	    _CBV_ViewDesc;
        D3D12_UNORDERED_ACCESS_VIEW_DESC	_UAV_ViewDesc;
    };

    float _clearColor[4];
    Vector2 _size = Vector2(0,0);
    int mipLevels = 1;
    DXGI_FORMAT imageFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
};
