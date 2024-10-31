#pragma once

#include <stdafx.h>
#include <Vertex.h>

namespace dxe
{
    class Mesh : public std::enable_shared_from_this<Mesh>
    {
    public:
        std::string name;

        std::vector<Vertex> _vertexList;
        std::vector<float> _dataBuffer;
        std::vector<uint32_t> _indexBuffer;

        BoundingBox _bound;

        Mesh();
        virtual ~Mesh();
        void SetName(const std::string& name);
        void SetBound(const BoundingBox& bound);
        void SetBound(const Vector3& min, const Vector3& max);
        void CalculateBound();
        void Init(std::vector<Vertex> _vertexList, std::vector<uint32_t> _indexBuffer);
        bool Intersects(std::shared_ptr<Transform> trans, const Ray& worldRay, float& dis);
        bool Intersects(std::shared_ptr<Transform> trans, const BoundingBox& worldBox);

        int indexCount = 0;
        int vertexCount = 0;
        int vertexStride = 0;

        bool enableIndex = true;

        void CreateBothBuffer();
        void CreateVertexBuffer();
        void CreateIndexBuffer();


        D3D12_VERTEX_BUFFER_VIEW	_vertexBufferView = {};
        D3D12_INDEX_BUFFER_VIEW		_indexBufferView = {};

        ComPtr<ID3D12Resource>		_vertexResource;
        ComPtr<ID3D12Resource>		_indexResource;
    };
}

