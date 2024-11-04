#include "stdafx.h"
#include "Mesh.h"

#include "GameObject.h"
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

void Mesh::SetSubIndex(int index)
{
    subMeshIndex = index;
}

void Mesh::SetBound(const BoundingBox& bound)
{
    this->_bound = bound;
}

void Mesh::SetBound(const Vector3& min, const Vector3& max)
{
    Vector3 center = (max + min) / 2;
    Vector3 extend = (max - min) / 2;
    BoundingBox bound;
    bound.Center = center;
    bound.Extents = extend;
    bound.Extents = Vector3(std::abs(bound.Extents.x), std::abs(bound.Extents.y), std::abs(bound.Extents.z));
    SetBound(bound);
}

void Mesh::CalculateBound()
{
    Vector3 min = Vector3(100,100,100);
    Vector3 max = -Vector3(100, 100, 100);
    for(auto& vert : _vertexList)
    {
        if (min.x >= vert.position.x)
            min.x = vert.position.x;
        if (min.y >= vert.position.y)
            min.y = vert.position.y;
        if (min.z >= vert.position.z)
            min.z = vert.position.z;

        if (max.x <= vert.position.x)
            max.x = vert.position.x;
        if (max.y <= vert.position.y)
            max.y = vert.position.y;
        if (max.z <= vert.position.z)
            max.z = vert.position.z;
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

bool Mesh::Intersects(std::shared_ptr<Transform> trans, const Ray& worldRay, float& dis, Vector3& normal)
{
    BoundingOrientedBox finalBound;
    BoundingOrientedBox obb = { this->_bound.Center, this->_bound.Extents, Vector4(0,0,0,1)};
    Matrix localToWorld = Matrix::Identity;
    if (trans != nullptr) {
        trans->GetLocalToWorldMatrix_BottomUp(localToWorld);
        obb.Transform(finalBound, localToWorld);
    }
    bool hit = finalBound.Intersects(worldRay.position, worldRay.direction, dis);
    if (hit && dis >= 0)
    {
        auto intersectionPoint = worldRay.position + worldRay.direction * dis;
        auto intersectionPoint2 = intersectionPoint;

        Matrix localToWorld2;
        localToWorld.Invert(localToWorld2);
        intersectionPoint2 = Vector3::Transform(intersectionPoint, localToWorld2);

        XMVECTOR normals[6] = {
        XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),   // +X
        XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f),  // -X
        XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),   // +Y
        XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f),  // -Y
        XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),   // +Z
        XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f)   // -Z
        };

        // 변환 행렬에서 회전 성분을 추출
        XMMATRIX obbRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&obb.Orientation));

        // 가장 가까운 면의 법선을 찾기 위해 각 면의 거리를 비교
        float maxDot = -FLT_MAX;
        for (int i = 0; i < 6; ++i)
        {
            // 법선 벡터를 회전 변환하여 OBB의 회전을 반영
            Vector3 rotatedNormal = XMVector3TransformNormal(normals[i], obbRotation);

            // 교차 지점에서의 법선 벡터는 레이 방향과 가장 일치하는 면의 법선
            Vector3 pointToCenter = intersectionPoint2 - Vector3(obb.Center);
            float dotProduct = pointToCenter.Dot(rotatedNormal);

            if (dotProduct > maxDot)
            {
                maxDot = dotProduct;
                normal = rotatedNormal;
            }
        }
        normal.Normalize(normal);
        normal = Vector3::TransformNormal(normal, localToWorld);
    }

    return hit;
}

bool Mesh::Intersects(std::shared_ptr<Transform> trans, const BoundingBox& worldBox)
{
    BoundingBox finalBound = this->_bound;

    if (trans != nullptr) {
        Matrix localToWorld = Matrix::Identity;
        trans->GetLocalToWorldMatrix_BottomUp(localToWorld);
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
    auto& vertexInto = GraphicManager::main->vertexInfo_Full;
    vertexCount = _vertexList.size();
    vertexStride = vertexInto.totalByteSize;
    auto bufferSize = vertexStride * vertexCount;
    _dataBuffer.resize(vertexInto.totalSize * vertexCount);

    int offset = 0;
    for(auto& vertex : _vertexList)
        vertex.WriteBuffer(_dataBuffer.data(), offset, vertexInto.props);

    {
        auto resourceCommandList = GraphicManager::main->GetResourceCommandList();

        // 텍스쳐용 리소스 생성
        auto uploadInfo = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(static_cast<size_t>(bufferSize));

        DXAssert(GraphicManager::main->_device->CreateCommittedResource(
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
        GraphicManager::main->_device->CreateCommittedResource(
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
        GraphicManager::main->GetResourceCommandList()->ResourceBarrier(1, &barrier);
        GraphicManager::main->GetResourceCommandList()->CopyResource(_vertexResource.Get(), _uploadBuffer.Get());
        D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(_vertexResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        GraphicManager::main->GetResourceCommandList()->ResourceBarrier(1, &barrier2);
        GraphicManager::main->SetResource(); // 어차피 여기서 execute 할거기 때문에 업로드 힙 없애도 됨 ㅇㅇ..
    }


    _vertexBufferView.BufferLocation = _vertexResource->GetGPUVirtualAddress();
    _vertexBufferView.StrideInBytes = vertexStride;
    _vertexBufferView.SizeInBytes = static_cast<size_t>(bufferSize);
}

void Mesh::CreateIndexBuffer()
{
    indexCount = _indexBuffer.size();
    auto bufferSize = indexCount * sizeof(uint32_t);
    GraphicManager::main->SetResource();
    {
        auto resourceCommandList = GraphicManager::main->GetResourceCommandList();

        // 텍스쳐용 리소스 생성
        auto uploadInfo = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(static_cast<size_t>(bufferSize));

        DXAssert(GraphicManager::main->_device->CreateCommittedResource(
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
        GraphicManager::main->_device->CreateCommittedResource(
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
        GraphicManager::main->GetResourceCommandList()->ResourceBarrier(1, &barrier);
        GraphicManager::main->GetResourceCommandList()->CopyResource(_indexResource.Get(), _uploadBuffer.Get());
        D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(_indexResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
        GraphicManager::main->GetResourceCommandList()->ResourceBarrier(1, &barrier2);
        GraphicManager::main->SetResource(); // 어차피 여기서 execute 할거기 때문에 업로드 힙 없애도 됨 ㅇㅇ..
    }

    _indexBufferView.BufferLocation = _indexResource->GetGPUVirtualAddress();
    _indexBufferView.Format = DXGI_FORMAT_R32_UINT;
    _indexBufferView.SizeInBytes = static_cast<size_t>(bufferSize);
}
