#include "stdafx.h"
#include "CBuffer.h"

#include "GraphicManager.h"
#include "graphic_config.h"

void CBufferView::SetData(const void* data, int size) const
{
    assert(size <= _cbufferSize);
    std::memcpy(_cbufferAddress, data, size);
}

CBuffer::CBuffer()
{
}

CBuffer::~CBuffer()
{
    if (_cbufferResource != nullptr)
        _cbufferResource->Unmap(0, nullptr);
    _cbufferResource = nullptr;
}

void CBuffer::Init(int bufferSize, int count)
{
    CreateCBufferResource(bufferSize, count);
    CreateCBufferView(count);
}

void CBuffer::CreateCBufferResource(int bufferSize, int count)
{
    bufferSize = (bufferSize + 255) & ~255;
    count = std::max(1, count);
    auto totalBufferSize = bufferSize * count;

    _cbufferStrideSize = bufferSize;
    _cbufferCount = count;
    _cbufferTotalSize = totalBufferSize;

    _readRange.Begin = 0;
    _readRange.End = _cbufferTotalSize;

    D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(_cbufferTotalSize);

    GraphicManager::main->_device->CreateCommittedResource(
        &heapProperty,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        ComPtrIDAddr(_cbufferResource));

    //_cbufferResource->Map(0, &_readRange, reinterpret_cast<void**>(&_cbufferAddress));
    _cbufferResource->Map(0, nullptr, reinterpret_cast<void**>(&_cbufferAddress));

    // 크기와 범위 잡고 Map으로 Cpu에서 접근할 address를 준비한다.
}

void CBuffer::CreateCBufferView(int count)
{
    D3D12_DESCRIPTOR_HEAP_DESC cbufferDHDesc = {};
    cbufferDHDesc.NumDescriptors = count;
    cbufferDHDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    cbufferDHDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    GraphicManager::main->_device->CreateDescriptorHeap(&cbufferDHDesc, IID_PPV_ARGS(&_cbufferDescriptorHeap));

    auto cpuHandleBegin = _cbufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    auto handleSize = GraphicManager::main->_device->GetDescriptorHandleIncrementSize(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    _cbufferCPUDescriptHandle.resize(count);
    for (int i = 0; i < count; i++)
    {
        _cbufferCPUDescriptHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuHandleBegin, i, handleSize);
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = _cbufferResource->GetGPUVirtualAddress() + static_cast<unsigned long long int>(
            _cbufferStrideSize) * i;
        cbvDesc.SizeInBytes = _cbufferStrideSize;

        GraphicManager::main->_device->CreateConstantBufferView(&cbvDesc, _cbufferCPUDescriptHandle[i]);
    }
}

CBufferView CBuffer::GetView(int index)
{
    CBufferView view;
    view.cbuffer = this;
    view.index = index;
    view.handle = GetHandle(index);

    view._cbufferOriginalAddress = _cbufferAddress;
    view._cbufferAddress = _cbufferAddress + (index * _cbufferStrideSize);
    view._cbufferSize = _cbufferStrideSize;
    return view;
}

D3D12_CPU_DESCRIPTOR_HANDLE CBuffer::GetHandle(int index) const
{
    assert(index < _cbufferCPUDescriptHandle.size());
    return _cbufferCPUDescriptHandle[index];
}
