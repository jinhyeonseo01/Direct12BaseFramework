#pragma once
#include <stdafx.h>

#include "Shader.h"


class DescriptorTable
{
public:
    DescriptorTable();
    virtual ~DescriptorTable();

    void Init(const std::vector<D3D12_DESCRIPTOR_RANGE>& descRange, int maxGroupCount);
    void Reset();
    int _descriptorHandleSize = 0;
    int _descriptorHeapTotalCount = 0;

    int _groupCount = 0;
    //int _groupIndex = 0;
    int _currentGroupIndex = 0;
    int _groupHandleCount = 0;

    std::unordered_map<std::string, int> _registerToHandleOffsetTable;

    ComPtr<ID3D12DescriptorHeap> _descriptorHeap;

    void SetCurrentGroupHandle(std::shared_ptr<Shader> shader, std::string registerName, const D3D12_CPU_DESCRIPTOR_HANDLE& handle);
    void SetCPUHandle(int groupIndex, int offsetIndex, const D3D12_CPU_DESCRIPTOR_HANDLE& handle);
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(int groupIndex, int offsetIndex);
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(int groupIndex, int offsetIndex);

    void SetCurrentGroupHandle(int offsetIndex, const D3D12_CPU_DESCRIPTOR_HANDLE& handle);
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentGroupCPUHandle(int offsetIndex);
    D3D12_GPU_DESCRIPTOR_HANDLE GetCurrentGroupGPUHandle(int offsetIndex);

    void SetNextGroupHandle();
};

