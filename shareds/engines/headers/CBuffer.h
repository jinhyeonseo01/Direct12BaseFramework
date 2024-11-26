#pragma once

#include <stdafx.h>

class CBuffer;

class CBufferView
{
public:
    int index;
    CBuffer* cbuffer;
    D3D12_CPU_DESCRIPTOR_HANDLE handle;
    unsigned char* _cbufferAddress = nullptr;
    unsigned char* _cbufferOriginalAddress = nullptr;
    int _cbufferSize = 0;

    void SetData(const void* data, int size) const;
};


class CBuffer
{
public:
    CBuffer();
    virtual ~CBuffer();

    ComPtr<ID3D12Resource> _cbufferResource;
    D3D12_RANGE _readRange;

    int _cbufferStrideSize = 0;
    int _cbufferCount = 0;
    int _cbufferTotalSize = 0;
    unsigned char* _cbufferAddress = nullptr;

    void Init(int bufferSize, int count = 1);
    void CreateCBufferResource(int bufferSize, int count = 1);
    void CreateCBufferView(int count = 1);

    CBufferView GetView(int index);

    D3D12_CPU_DESCRIPTOR_HANDLE GetHandle(int index) const;

public:
    ComPtr<ID3D12DescriptorHeap> _cbufferDescriptorHeap;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> _cbufferCPUDescriptHandle;
};
