#pragma once
#include <stdafx.h>
#include <string>

class Texture
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
    void Load(const std::wstring& path);


    ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
    ComPtr<ID3D12DescriptorHeap>	_dsvHeap;

    D3D12_CPU_DESCRIPTOR_HANDLE		_SRVCPUHandle{};

    float _clearColor[4];
};

