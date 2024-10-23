#include "CBuffer.h"

#include "GraphicManager.h"
#include "graphic_config.h"

CBuffer::CBuffer()
{
}

CBuffer::~CBuffer()
{
    if(_cbufferResource != nullptr)
        _cbufferResource->Unmap(0, nullptr);
    _cbufferResource = nullptr;
}

void CBuffer::Init(int bufferSize, int count)
{
    CreateCBufferResource(bufferSize, count);

    int index = 0;
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = _cbufferResource->GetGPUVirtualAddress() + static_cast<unsigned long long int>(_cbufferSize) * index;
    cbvDesc.SizeInBytes = _cbufferSize;
}

void CBuffer::CreateCBufferResource(int bufferSize, int count)
{
    bufferSize = (bufferSize + 255) & ~255;
    count = std::min(1, count);
    auto totalBufferSize = bufferSize * count;

    _cbufferSize = bufferSize;
    _cbufferCount = count;
    _cbufferTotalSize = totalBufferSize;

    _readRange.Begin = 0;
    _readRange.End = _cbufferTotalSize;

    D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(_cbufferTotalSize);

    GraphicManager::instance->_device->CreateCommittedResource(
        &heapProperty,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        ComPtrIDAddr(_cbufferResource));

    _cbufferResource->Map(0, &_readRange, reinterpret_cast<void**>(&_cbufferAddress));

    // 크기와 범위 잡고 Map으로 Cpu에서 접근할 address를 준비한다.
}
