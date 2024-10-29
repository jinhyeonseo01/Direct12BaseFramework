#pragma once

#include <stdafx.h>

#include "CBuffer.h"
#include "Shader.h"

class CBufferPool
{
public:
    CBufferPool();
    virtual ~CBufferPool();

    int _cbufferDescriptorSize = 0;
    int _cbufferDescriptorHeapCount = 0;
    int _cbufferDescriptorHeapNextIndex = 0;

    //std::vector<bool> _cbufferDescriptorHeapAllocator;
    //ComPtr<ID3D12DescriptorHeap> _cbufferDescriptorHeap;

    std::unordered_map<std::string, std::shared_ptr<CBuffer>> _cbufferTable;
    //std::unordered_map<std::string, int> _cbufferTable;


    static std::unordered_map<std::string, ShaderCBufferInfo> _cbufferInfoTable;
    static std::unordered_map<std::string, int> _cbufferIndexTable;


    static void CBufferRegister(std::string name, int cbufferSize, int count);
    static void CBufferRegister(ShaderCBufferInfo& cbufferInfo, int count);
    void AddCBuffer(std::string name, std::shared_ptr<CBuffer> cbuffer);
    void AddCBuffer(std::string name, int size, int count);
    CBufferView PopCBuffer(std::string name);
    void Init();
    void Reset();

    //uint32_t CBufferIndexAlloc();
    //D3D12_CPU_DESCRIPTOR_HANDLE CBufferDescriptorHandleAlloc(D3D12_CPU_DESCRIPTOR_HANDLE* handle = nullptr);
    //void CBufferDescriptorHandleFree(const D3D12_CPU_DESCRIPTOR_HANDLE& handle);

};

