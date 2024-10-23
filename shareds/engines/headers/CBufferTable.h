#pragma once

#include <stdafx.h>

#include "CBuffer.h"

class CBufferTable
{
public:
    CBufferTable();
    virtual ~CBufferTable();

    int _cbufferDescriptorSize = 0;
    int _cbufferDescriptorHeapCount = 0;
    int _cbufferDescriptorHeapNextIndex = 0;

    std::vector<bool> _cbufferDescriptorHeapAllocator;

    std::shared_ptr<CBuffer> _cbuffer = nullptr;


    void Init();
    void Reset();

    uint32_t CBufferIndexAlloc();
    D3D12_CPU_DESCRIPTOR_HANDLE CBufferDescriptorHandleAlloc(D3D12_CPU_DESCRIPTOR_HANDLE* handle = nullptr);
    void CBufferDescriptorHandleFree(const D3D12_CPU_DESCRIPTOR_HANDLE& handle);

};

