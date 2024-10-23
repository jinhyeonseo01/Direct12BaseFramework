#include "CBufferTable.h"

#include "GraphicManager.h"
#include "graphic_config.h"


CBufferTable::CBufferTable()
{

}

CBufferTable::~CBufferTable()
{

}

void CBufferTable::Init()
{
    _cbuffer = std::make_shared<CBuffer>();
    _cbuffer->InitOnlyResource(500, 1);

    _cbufferDescriptorSize = GraphicManager::instance->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    _cbufferDescriptorHeapAllocator.resize(_cbufferDescriptorHeapCount, false);

    D3D12_DESCRIPTOR_HEAP_DESC SRV_HeapDesc = {};
    SRV_HeapDesc.NumDescriptors = _cbufferDescriptorHeapCount;
    SRV_HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    SRV_HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  // Ensure shader visibility if needed

    DXAssert(GraphicManager::instance->_device->CreateDescriptorHeap(&SRV_HeapDesc, IID_PPV_ARGS(&_cbufferDescriptorHeap)));


}

void CBufferTable::Reset()
{

}

uint32_t CBufferTable::CBufferIndexAlloc()
{
    for (int i = _cbufferDescriptorHeapNextIndex; i < _cbufferDescriptorHeapAllocator.size(); ++i)
    {
        if (!_cbufferDescriptorHeapAllocator[i])
        {
            _cbufferDescriptorHeapAllocator[i] = true;
            _cbufferDescriptorHeapNextIndex = (i + 1) % _cbufferDescriptorHeapAllocator.size();
            return i;
        }
    }
    for (int i = 0; i < _cbufferDescriptorHeapNextIndex; ++i)
    {
        if (!_cbufferDescriptorHeapAllocator[i])
        {
            _cbufferDescriptorHeapAllocator[i] = true;
            _cbufferDescriptorHeapNextIndex = (i + 1) % _cbufferDescriptorHeapAllocator.size();
            return i;
        }
    }
    return -1;  // No available slots
}

D3D12_CPU_DESCRIPTOR_HANDLE CBufferTable::CBufferDescriptorHandleAlloc(D3D12_CPU_DESCRIPTOR_HANDLE* handle)
{
    unsigned int index = CBufferIndexAlloc();
    assert(index != -1);

    CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHandle(_cbufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), index, _cbufferDescriptorSize);
    if (handle != nullptr)
        *handle = DescriptorHandle;
    return DescriptorHandle;
}

void CBufferTable::CBufferDescriptorHandleFree(const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
{
    D3D12_CPU_DESCRIPTOR_HANDLE start = _cbufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    uint32_t index = static_cast<uint32_t>(handle.ptr - start.ptr) / _cbufferDescriptorSize;
    assert(index >= 0);
    _cbufferDescriptorHeapAllocator[index] = false;
}

