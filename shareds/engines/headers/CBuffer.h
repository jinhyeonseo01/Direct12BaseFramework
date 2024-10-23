#pragma once

#include <stdafx.h>

class CBuffer
{
public:
    CBuffer();
    virtual ~CBuffer();

    ComPtr<ID3D12Resource> _cbufferResource;
    D3D12_RANGE _readRange;

    int _cbufferSize = 0;
    int _cbufferCount = 0;
    int _cbufferTotalSize = 0;
    unsigned char* _cbufferAddress = nullptr;

    void Init(int bufferSize, int count = 1);
    void CreateCBufferResource(int bufferSize, int count = 1);

public:
    ComPtr<ID3D12DescriptorHeap> _cbufferDescriptorHeap;
    
};

