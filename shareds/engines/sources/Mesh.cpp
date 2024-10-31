#include "stdafx.h"
#include "Mesh.h"

#include "GraphicManager.h"
#include "graphic_config.h"
#include "Transform.h"

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
    _vertexList.clear();
}

void Mesh::SetName(const std::string& name)
{
    this->name = name;
}

void Mesh::SetBound(const BoundingBox& bound)
{
    this->_bound = bound;
}

void Mesh::SetBound(const Vector3& min, const Vector3& max)
{
    Vector3 center = (max + min) / 2;
    BoundingBox bound;
    bound.Center = center;
    bound.Extents = max - center;
    bound.Extents = Vector3(std::abs(bound.Extents.x), std::abs(bound.Extents.y), std::abs(bound.Extents.z));
    SetBound(bound);
}

void Mesh::CalculateBound()
{
    Vector3 min = Vector3::Zero;
    Vector3 max = Vector3::Zero;
    for(auto& vert : _vertexList)
    {
        if (min.x > vert.position.x)
            min.x = vert.position.x;
        if (min.y > vert.position.y)
            min.y = vert.position.y;
        if (max.x < vert.position.x)
            max.x = vert.position.x;
        if (max.y < vert.position.y)
            max.y = vert.position.y;
    }
    SetBound(min, max);
}

void Mesh::Init(std::vector<Vertex> _vertexList, std::vector<uint32_t> _indexBuffer)
{
    this->_vertexList = _vertexList;
    this->_indexBuffer = _indexBuffer;
    CalculateBound();
    if (this->name.empty())
        SetName("none");
}

bool Mesh::Intersects(std::shared_ptr<Transform> trans, const Ray& worldRay, float& dis)
{
    BoundingBox finalBound = this->_bound;

    if (trans != nullptr) {
        Matrix localToWorld = Matrix::Identity;
        trans->GetLocalToWorldMatrix(localToWorld);
        this->_bound.Transform(finalBound, localToWorld);
    }
    return worldRay.Intersects(finalBound, dis);
}

bool Mesh::Intersects(std::shared_ptr<Transform> trans, const BoundingBox& worldBox)
{
    BoundingBox finalBound = this->_bound;

    if (trans != nullptr) {
        Matrix localToWorld = Matrix::Identity;
        trans->GetLocalToWorldMatrix(localToWorld);
        this->_bound.Transform(finalBound, localToWorld);
    }
    return worldBox.Intersects(finalBound);
}


void Mesh::CreateBothBuffer()
{
    CreateVertexBuffer();
    CreateIndexBuffer();
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
