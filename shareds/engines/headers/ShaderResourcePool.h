#pragma once
#include "stdafx.h"

class ShaderResourcePool
{
public:
    int _descriptorSize = 0;
    int _descriptorHeapCount = 0;
    int _descriptorHeapIndex = 0;
    std::vector<bool> _descriptorHeapAllocator;
    ComPtr<ID3D12DescriptorHeap> _descriptorHeap;


    void Init(int count, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    void Reset();
    uint32_t IndexAlloc();
    void IndexFree(uint32_t index);
    D3D12_CPU_DESCRIPTOR_HANDLE HandleAlloc();
    void HandleFree(const D3D12_CPU_DESCRIPTOR_HANDLE& handle);
};
