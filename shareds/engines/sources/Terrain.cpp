#include "stdafx.h"
#include "Terrain.h"


float Terrain::GetHeigth(int x, int y)
{
    return _map[x + y * width] / (float)heightSize;
}

std::shared_ptr<Terrain> Terrain::Load(const std::wstring& path, int xLength, int yLength, TerrainDataType dataType,
                                       int size, int height)
{
    std::ifstream is{path, std::ios::binary};
    if (is)
    {
        auto terrain = std::make_shared<Terrain>();
        std::vector<unsigned short> dataList;

        terrain->width = xLength;
        terrain->height = yLength;
        terrain->fieldSize = size;
        terrain->fieldSize = height;

        unsigned short data;
        unsigned char data2;
        for (int i = 0; i < xLength * yLength; i++)
        {
            bool tf = false;
            if (dataType == TerrainDataType::uint16)
                tf = static_cast<bool>(is >> data);
            if (dataType == TerrainDataType::uint8)
            {
                tf = static_cast<bool>(is >> data2);
                data = data2;
            }
            if (tf)
            {
                dataList.push_back(data);
            }
            else
                break;
        }
        terrain->_map = dataList;


        return terrain;
    }
    return nullptr;
}
