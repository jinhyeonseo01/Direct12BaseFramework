#pragma once
#include <stdafx.h>
#include <string>

enum class ResourceState
{
    RTV,
    DSV,
    SRV,
    RTVSRV,
};

class Texture : public std::enable_shared_from_this<Texture>
{

public:
    ComPtr<ID3D12Resource> GetResource() { return _resource; }
    ComPtr<ID3D12DescriptorHeap> GetRTV() { return _rtvHeap; }
    ComPtr<ID3D12DescriptorHeap> GetDSV() { return _dsvHeap; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() { return _SRVCPUHandle; }
    float* GetClearColor() { return _clearColor; }

    ResourceState					_state = ResourceState::SRV;
    ScratchImage			 		_image;
    ComPtr<ID3D12Resource>			_resource;

    static std::shared_ptr<Texture> Create(DXGI_FORMAT format, uint32_t width, uint32_t height, const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags, ResourceState state, Vector4 clearColor = Vector4(1,1,1,1));
    static std::shared_ptr<Texture> Load(const std::wstring& path);


    void CreateFromResource(ComPtr<ID3D12Resource> resource, DXGI_FORMAT format);


    ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
    ComPtr<ID3D12DescriptorHeap>	_dsvHeap;

    D3D12_CPU_DESCRIPTOR_HANDLE		_SRVCPUHandle{};

    float _clearColor[4];
};

