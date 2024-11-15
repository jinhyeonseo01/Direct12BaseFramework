#pragma once

#include <stdafx.h>
#include <Vertex.h>

namespace dxe
{
    class Mesh : public std::enable_shared_from_this<Mesh>
    {
    public:
        std::string name;
        int subMeshIndex = 0;

        std::vector<Vertex> _vertexList;
        std::vector<float> _dataBuffer;
        std::vector<uint32_t> _indexBuffer;

        BoundingBox _bound;

        Mesh();
        virtual ~Mesh();
        void SetName(const std::string& name);
        void SetSubIndex(int index);
        void SetBound(const BoundingBox& bound);
        void SetBound(const Vector3& min, const Vector3& max);
        BoundingOrientedBox GetOBB(const Quaternion& quat);
        BoundingOrientedBox GetWorldOBB(std::shared_ptr<Transform> trans);
        void CalculateBound();
        void Init(const std::vector<Vertex>& _vertexList, const std::vector<uint32_t>& _indexBuffer);
        bool Intersects(std::shared_ptr<Transform> trans, const Ray& worldRay, float& dis, Vector3& normal);
        bool Intersects(std::shared_ptr<Transform> trans, const BoundingOrientedBox& worldBox);

        int indexCount = 0;
        int vertexCount = 0;
        int vertexStride = 0;

        bool enableIndex = true;

        void CreateBothBuffer(D3D12_PRIMITIVE_TOPOLOGY indexTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        void CreateVertexBuffer();
        void CreateIndexBuffer(D3D12_PRIMITIVE_TOPOLOGY indexTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


        D3D12_VERTEX_BUFFER_VIEW _vertexBufferView = {};
        D3D12_INDEX_BUFFER_VIEW _indexBufferView = {};

        ComPtr<ID3D12Resource> _vertexResource;
        ComPtr<ID3D12Resource> _indexResource;

        D3D12_PRIMITIVE_TOPOLOGY _indexTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    };
}
