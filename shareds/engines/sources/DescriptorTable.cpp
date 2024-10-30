#include "stdafx.h"
#include "DescriptorTable.h"

#include "GraphicManager.h"
#include "graphic_config.h"

DescriptorTable::DescriptorTable()
{
}

DescriptorTable::~DescriptorTable()
{
}

void DescriptorTable::Init(const std::vector<D3D12_DESCRIPTOR_RANGE>& descRange, int maxGroupCount)
{
    int handleOffset = 0;
    for(int i=0;i< descRange.size();i++)
    {
        char semantic = 'b';
        if (descRange[i].RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_CBV)
            semantic = 'b';
        if (descRange[i].RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV)
            semantic = 't';
        if (descRange[i].RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_UAV)
            semantic = 'u';
        if (descRange[i].RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
            semantic = 's';

        for(int j=0;j< descRange[i].NumDescriptors;j++)
        {
            int registerOffset = descRange[i].BaseShaderRegister + j;
            std::string key = semantic + std::to_string(registerOffset);
            str::trim(key);
            _registerToHandleOffsetTable.emplace(key, handleOffset);
            ++handleOffset;
        }
    }
    _groupHandleCount = handleOffset; // 그룹의 핸들 갯수
    _groupCount = maxGroupCount; // 그룹 갯수
    _descriptorHeapTotalCount = _groupHandleCount * _groupCount;
    _descriptorHandleSize = GraphicManager::instance->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    D3D12_DESCRIPTOR_HEAP_DESC CBV_HeapDesc = {};
    CBV_HeapDesc.NumDescriptors = _descriptorHeapTotalCount;
    CBV_HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    CBV_HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;  // Ensure shader visibility if needed

    DXAssert(GraphicManager::instance->_device->CreateDescriptorHeap(&CBV_HeapDesc, ComPtrIDAddr(_descriptorHeap)));

    CBV_HeapDesc = {};
    CBV_HeapDesc.NumDescriptors = _groupHandleCount;
    CBV_HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    CBV_HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  // Ensure shader visibility if needed

    DXAssert(GraphicManager::instance->_device->CreateDescriptorHeap(&CBV_HeapDesc, ComPtrIDAddr(_tempDescriptorHeap)));
}

void DescriptorTable::Reset()
{
    _currentGroupIndex = 0;
    _recycleHandleTable.clear();
}

void DescriptorTable::AddRecycleHandle(std::string registerName, const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
{
    _recycleHandleTable[registerName] = handle;
}

void DescriptorTable::RecycleCurrentGroupHandle(std::shared_ptr<Shader> shader, std::string registerName)
{
    if(_recycleHandleTable.contains(registerName))
    {
        SetCurrentGroupHandle(shader, registerName, _recycleHandleTable[registerName]);
    }
    else
    {
        Debug::log << "재사용할 핸들 찾기 실패 \n";
    }
}

void DescriptorTable::SetCurrentGroupHandle(std::shared_ptr<Shader> shader, std::string registerName, const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
{
    auto& info = shader->_profileInfo;
    auto registerInfo = info.GetRegisterByName(registerName);
    AddRecycleHandle(registerName, handle);

    if(!registerInfo.name.empty() && _registerToHandleOffsetTable.contains(registerInfo.registerTypeString))
    {
        int handleOffset = _registerToHandleOffsetTable[registerInfo.registerTypeString];
        SetCPUHandle(_currentGroupIndex, handleOffset, handle);
    }
    else
    {
        //throw std::exception("레지스터 id 존재하지 않음.");
    }
}

void DescriptorTable::SetCPUHandle(int groupIndex, int offsetIndex, const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
{
    int offset = (_groupHandleCount * groupIndex + offsetIndex);
    CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), offset, _descriptorHandleSize);

    unsigned int destRange = 1;
    unsigned int srcRange = 1;
    GraphicManager::instance->_device->CopyDescriptors(
        1, &descriptorHandle, &destRange,
        1, &handle, &srcRange,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE tempDescriptorHandle(_tempDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), offsetIndex, _descriptorHandleSize);
    GraphicManager::instance->_device->CopyDescriptors(
        1, &tempDescriptorHandle, &destRange,
        1, &handle, &srcRange,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorTable::GetCPUHandle(int groupIndex, int offsetIndex)
{
    int offset = (_groupHandleCount * groupIndex + offsetIndex);
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), offset, _descriptorHandleSize);
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorTable::GetGPUHandle(int groupIndex, int offsetIndex)
{
    int offset = (_groupHandleCount * groupIndex + offsetIndex);
    return CD3DX12_GPU_DESCRIPTOR_HANDLE(_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), offset, _descriptorHandleSize);
}

void DescriptorTable::SetCurrentGroupHandle(int offsetIndex, const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
{
    SetCPUHandle(_currentGroupIndex, offsetIndex, handle);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorTable::GetCurrentGroupCPUHandle(int offsetIndex)
{
    return GetCPUHandle(_currentGroupIndex, offsetIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorTable::GetCurrentGroupGPUHandle(int offsetIndex)
{
    int offset = (_groupHandleCount * _currentGroupIndex + offsetIndex);
    return  CD3DX12_GPU_DESCRIPTOR_HANDLE(_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), offset, _descriptorHandleSize);
}

void DescriptorTable::SetNextGroupHandle()
{
    _currentGroupIndex++;
    assert(_currentGroupIndex < _groupHandleCount);

    unsigned int destRange = _groupHandleCount;
    unsigned int srcRange = _groupHandleCount;

    CD3DX12_CPU_DESCRIPTOR_HANDLE prevDescriptorHandle(_tempDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 0, _descriptorHandleSize);
    CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), _groupHandleCount * _currentGroupIndex, _descriptorHandleSize);
    GraphicManager::instance->_device->CopyDescriptors(
        1, &descriptorHandle, &destRange,
        1, &prevDescriptorHandle, &srcRange,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
