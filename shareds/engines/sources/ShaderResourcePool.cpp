#include "stdafx.h"
#include "ShaderResourcePool.h"

#include "GraphicManager.h"
#include "graphic_config.h"


void ShaderResourcePool::Init(int count, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
    _descriptorHeapCount = count;
    _descriptorSize = GraphicManager::instance->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    _descriptorHeapAllocator.resize(_descriptorHeapCount);
    for (int i = 0; i < _descriptorHeapCount; i++)
        _descriptorHeapAllocator[i] = false;
    _descriptorHeapIndex = 0;

    D3D12_DESCRIPTOR_HEAP_DESC SRV_HeapDesc = {};
    SRV_HeapDesc.NumDescriptors = _descriptorHeapCount;
    SRV_HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    SRV_HeapDesc.Flags = flags;  // Ensure shader visibility if needed

    DXAssert(GraphicManager::instance->_device->CreateDescriptorHeap(&SRV_HeapDesc, ComPtrIDAddr(_descriptorHeap)));
}

void ShaderResourcePool::Reset()
{
    for (int i = 0; i < _descriptorHeapCount; i++)
        _descriptorHeapAllocator[i] = false;
    _descriptorHeapIndex = 0;
}

uint32_t ShaderResourcePool::IndexAlloc()
{
    for (int i = _descriptorHeapIndex + 1; i < _descriptorHeapAllocator.size(); ++i)
    {
        if (!_descriptorHeapAllocator[i])
        {
            _descriptorHeapAllocator[i] = true;
            _descriptorHeapIndex = i;
            return static_cast<uint32_t>(i);
        }
    }
    for (int i = 0; i < _descriptorHeapIndex + 1; ++i)
    {
        if (!_descriptorHeapAllocator[i])
        {
            _descriptorHeapAllocator[i] = true;
            _descriptorHeapIndex = i;
            return static_cast<uint32_t>(i);
        }
    }
    return -1;  // No available slots
}

void ShaderResourcePool::IndexFree(uint32_t index)
{
    _descriptorHeapAllocator[index] = false;
}

D3D12_CPU_DESCRIPTOR_HANDLE ShaderResourcePool::HandleAlloc()
{
    auto index = IndexAlloc();
    assert(index != -1);
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), index, _descriptorSize);
}

void ShaderResourcePool::HandleFree(const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
{
    D3D12_CPU_DESCRIPTOR_HANDLE start = _descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    uint32_t index = static_cast<uint32_t>(static_cast<unsigned long int>(handle.ptr - start.ptr) / _descriptorSize);
    assert(index >= 0 && _descriptorHeapAllocator.size() > index);
    IndexFree(index);
}
