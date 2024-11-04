#pragma once

#include <stdafx.h>
#include <Mesh.h>

class MeshSample
{
public:
    static std::shared_ptr<Mesh> CreateCubeMap();
    static std::shared_ptr<Mesh> CreateQuad();
    static std::shared_ptr<Mesh> CreateTerrain(std::vector<unsigned short> arrayData, int xCount, int yCount, int size = 100, int height = 100);
};

