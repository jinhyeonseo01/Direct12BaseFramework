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
    std::unordered_map<std::string, D3D12_CPU_DESCRIPTOR_HANDLE> _recycleHandleTable;
    std::vector<int> _textureHandleFields;

    ComPtr<ID3D12DescriptorHeap> _descriptorHeap;
    ComPtr<ID3D12DescriptorHeap> _tempDescriptorHeap;

    void AddRecycleHandle(std::string registerName, const D3D12_CPU_DESCRIPTOR_HANDLE& handle);
    void SetCurrentGroupHandle(std::shared_ptr<Shader> shader, std::string registerName,
                               const D3D12_CPU_DESCRIPTOR_HANDLE& handle);
    void RecycleCurrentGroupHandle(std::shared_ptr<Shader> shader, std::string registerName);
    void SetCPUHandle(int groupIndex, int offsetIndex, const D3D12_CPU_DESCRIPTOR_HANDLE& handle);
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(int groupIndex, int offsetIndex);
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(int groupIndex, int offsetIndex);

    void SetCurrentGroupHandle(int offsetIndex, const D3D12_CPU_DESCRIPTOR_HANDLE& handle);
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentGroupCPUHandle(int offsetIndex);
    D3D12_GPU_DESCRIPTOR_HANDLE GetCurrentGroupGPUHandle(int offsetIndex);

    void SetNextGroupHandle();
};
