#pragma once

#include <stdafx.h>
#include <Vertex.h>

namespace dxe
{
    class Mesh : public std::enable_shared_from_this<Mesh>
    {
    public:
        std::vector<Vertex> _vertexList;
        Mesh();
        virtual ~Mesh();

        int indexCount = 0;
        int vertexCount = 0;
        int vertexStride = 0;

        bool enableIndex = true;

        std::vector<float> _dataBuffer;
        std::vector<uint32_t> _indexBuffer;

        void CreateVertexBuffer();
        void CreateIndexBuffer();


        D3D12_VERTEX_BUFFER_VIEW	_vertexBufferView = {};
        D3D12_INDEX_BUFFER_VIEW		_indexBufferView = {};

        ComPtr<ID3D12Resource>		_vertexResource;
        ComPtr<ID3D12Resource>		_indexResource;
    };
}

