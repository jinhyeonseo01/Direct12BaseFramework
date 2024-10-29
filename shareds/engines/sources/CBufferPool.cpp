#include "CBufferPool.h"

#include "GraphicManager.h"
#include "graphic_config.h"


std::unordered_map<std::string, ShaderCBufferInfo> CBufferPool::_cbufferInfoTable{};
std::unordered_map<std::string, int> CBufferPool::_cbufferIndexTable{};

CBufferPool::CBufferPool()
{

}

CBufferPool::~CBufferPool()
{

}

void CBufferPool::CBufferRegister(std::string name, int cbufferSize, int count)
{
    ShaderCBufferInfo info{};
    info.name = name;
    info.bufferByteSize = cbufferSize;
    CBufferRegister(info, count);
}

void CBufferPool::CBufferRegister(ShaderCBufferInfo& cbufferInfo, int count)
{
    if(!_cbufferInfoTable.contains(cbufferInfo.name)) {
        _cbufferInfoTable.emplace(cbufferInfo.name, cbufferInfo);
    }
    if(!_cbufferIndexTable.contains(cbufferInfo.name))
        _cbufferIndexTable.emplace(cbufferInfo.name, count);
    _cbufferIndexTable[cbufferInfo.name] = count;
}

void CBufferPool::AddCBuffer(std::string name, std::shared_ptr<CBuffer> cbuffer)
{
    _cbufferTable.emplace(name, cbuffer);
    _cbufferIndexTable.emplace(name, 0);
}

void CBufferPool::AddCBuffer(std::string name, int size, int count)
{
    auto cbuffer = std::make_shared<CBuffer>();
    cbuffer->Init(size, count);
    _cbufferTable.emplace(name, cbuffer);
    _cbufferIndexTable.emplace(name, 0);
}

CBufferView CBufferPool::PopCBuffer(std::string name)
{
    auto view = _cbufferTable[name]->GetView(_cbufferIndexTable[name]);
    _cbufferIndexTable[name] += 1;
    return view;
}

void CBufferPool::Init()
{
    for(auto& cbufferInfo : _cbufferInfoTable)
    {
        Debug::log << (cbufferInfo.second.name) << "\n";
    }
    _cbufferDescriptorSize = GraphicManager::instance->_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CBufferPool::Reset()
{
    for (auto& indexPair : _cbufferIndexTable)
        _cbufferIndexTable[indexPair.first] = 0;
}
/*
uint32_t CBufferPool::CBufferIndexAlloc()
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

D3D12_CPU_DESCRIPTOR_HANDLE CBufferPool::CBufferDescriptorHandleAlloc(D3D12_CPU_DESCRIPTOR_HANDLE* handle)
{
    unsigned int index = CBufferIndexAlloc();
    assert(index != -1);

    CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHandle(_cbufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), index, _cbufferDescriptorSize);

    if (handle != nullptr)
        *handle = DescriptorHandle;
    return DescriptorHandle;
}

void CBufferPool::CBufferDescriptorHandleFree(const D3D12_CPU_DESCRIPTOR_HANDLE& handle)
{
    D3D12_CPU_DESCRIPTOR_HANDLE start = _cbufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    uint32_t index = static_cast<uint32_t>(handle.ptr - start.ptr) / _cbufferDescriptorSize;
    assert(index >= 0);
    _cbufferDescriptorHeapAllocator[index] = false;
}
*/
