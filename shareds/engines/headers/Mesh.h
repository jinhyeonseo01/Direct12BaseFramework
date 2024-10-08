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
    };
}

