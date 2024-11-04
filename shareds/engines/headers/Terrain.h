#pragma once
#include <stdafx.h>
#include <GraphicManager.h>

namespace dxe
{
    enum class TerrainDataType
    {
        uint8,
        uint16
    };
    class Terrain
    {
    public:
        std::wstring path;
        std::vector<unsigned short> _map;
        int width = 0;
        int height = 0;
        int heightSize = 0;
        int fieldSize = 0;

        std::shared_ptr<Mesh> mesh;

        float GetHeigth(int x, int y);

        static std::shared_ptr<Terrain> Load(const std::wstring& path, int xLength = 0, int yLength = 0, TerrainDataType dataType = TerrainDataType::uint16, int size = 100, int height = 100);
    };
}

