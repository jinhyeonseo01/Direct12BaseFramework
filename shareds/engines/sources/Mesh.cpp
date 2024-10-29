#include "Mesh.h"

#include "GraphicManager.h"
#include "graphic_config.h"

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
    _vertexList.clear();
}

void Mesh::CreateVertexBuffer()
{
    auto& vertexInto = GraphicManager::instance->vertexInfo_Full;
    vertexCount = _vertexList.size();
    vertexStride = vertexInto.totalByteSize;
    auto bufferSize = vertexStride * vertexCount;
    _dataBuffer.resize(vertexInto.totalSize * vertexCount);

    int offset = 0;
    for(auto& vertex : _vertexList)
        vertex.WriteBuffer(_dataBuffer.data(), offset, vertexInto.props);

    {
        auto resourceCommandList = GraphicManager::instance->GetResourceCommandList();

        // 텍스쳐용 리소스 생성
        auto uploadInfo = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(static_cast<size_t>(bufferSize));

        DXAssert(GraphicManager::instance->_device->CreateCommittedResource(
            &uploadInfo,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr, // clear value가 없으므로 null
            ComPtrIDAddr(_vertexResource)
        ));
    }

    {
        ComPtr<ID3D12Resource> _uploadBuffer;
        auto uploadInfo = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(static_cast<size_t>(bufferSize));
        GraphicManager::instance->_device->CreateCommittedResource(
            &uploadInfo,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            ComPtrIDAddr(_uploadBuffer));

        CD3DX12_RANGE readRange(0, 0);
        unsigned char* data;
        _uploadBuffer->Map(0, &readRange, reinterpret_cast<void**>(&data));
        ::memcpy(data, _dataBuffer.data(), static_cast<size_t>(bufferSize));
        _uploadBuffer->Unmap(0, nullptr);


        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(_vertexResource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
        GraphicManager::instance->GetResourceCommandList()->ResourceBarrier(1, &barrier);
        GraphicManager::instance->GetResourceCommandList()->CopyResource(_vertexResource.Get(), _uploadBuffer.Get());
        D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(_vertexResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        GraphicManager::instance->GetResourceCommandList()->ResourceBarrier(1, &barrier2);
        GraphicManager::instance->SetResource(); // 어차피 여기서 execute 할거기 때문에 업로드 힙 없애도 됨 ㅇㅇ..
    }


    _vertexBufferView.BufferLocation = _vertexResource->GetGPUVirtualAddress();
    _vertexBufferView.StrideInBytes = vertexStride;
    _vertexBufferView.SizeInBytes = static_cast<size_t>(bufferSize);
}

void Mesh::CreateIndexBuffer()
{
    indexCount = _indexBuffer.size();
    auto bufferSize = indexCount * sizeof(uint32_t);
    GraphicManager::instance->SetResource();
    {
        auto resourceCommandList = GraphicManager::instance->GetResourceCommandList();

        // 텍스쳐용 리소스 생성
        auto uploadInfo = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(static_cast<size_t>(bufferSize));

        DXAssert(GraphicManager::instance->_device->CreateCommittedResource(
            &uploadInfo,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr, // clear value가 없으므로 null
            ComPtrIDAddr(_indexResource)
        ));
    }

    {
        ComPtr<ID3D12Resource> _uploadBuffer;
        auto uploadInfo = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(static_cast<size_t>(bufferSize));
        GraphicManager::instance->_device->CreateCommittedResource(
            &uploadInfo,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            ComPtrIDAddr(_uploadBuffer));

        CD3DX12_RANGE readRange(0, 0);
        unsigned char* data;
        _uploadBuffer->Map(0, &readRange, reinterpret_cast<void**>(&data));
        ::memcpy(data, _indexBuffer.data(), static_cast<size_t>(bufferSize));
        _uploadBuffer->Unmap(0, nullptr);


        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(_vertexResource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
        GraphicManager::instance->GetResourceCommandList()->ResourceBarrier(1, &barrier);
        GraphicManager::instance->GetResourceCommandList()->CopyResource(_indexResource.Get(), _uploadBuffer.Get());
        D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(_indexResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
        GraphicManager::instance->GetResourceCommandList()->ResourceBarrier(1, &barrier2);
        GraphicManager::instance->SetResource(); // 어차피 여기서 execute 할거기 때문에 업로드 힙 없애도 됨 ㅇㅇ..
    }

    _indexBufferView.BufferLocation = _indexResource->GetGPUVirtualAddress();
    _indexBufferView.Format = DXGI_FORMAT_R32_UINT;
    _indexBufferView.SizeInBytes = static_cast<size_t>(bufferSize);
}
