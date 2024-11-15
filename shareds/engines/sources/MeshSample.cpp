#include "stdafx.h"
#include "MeshSample.h"

std::shared_ptr<Mesh> MeshSample::CreateCubeMap()
{
    return nullptr;
}

std::shared_ptr<Mesh> MeshSample::CreateQuad()
{
    auto mesh = std::make_shared<Mesh>();

    std::vector<Vertex> verts;
    std::vector<unsigned int> inds;
    Vertex vert;
    vert.position = Vector3(-0.5, 0.5, 0);
    vert.uvs[0] = Vector3(0, 0, 0);
    vert.normal = Vector3(0, 0, -1);
    vert.tangent = Vector3(0, 1, 0);
    vert.bitangent = Vector3(1, 0, 0);
    verts.push_back(vert);

    vert.position = Vector3(0.5, 0.5, 0);
    vert.uvs[0] = Vector3(1, 0, 0);
    vert.normal = Vector3(0, 0, -1);
    vert.tangent = Vector3(0, 1, 0);
    vert.bitangent = Vector3(1, 0, 0);
    verts.push_back(vert);

    vert.position = Vector3(0.5, -0.5, 0);
    vert.uvs[0] = Vector3(1, 1, 0);
    vert.normal = Vector3(0, 0, -1);
    vert.tangent = Vector3(0, 1, 0);
    vert.bitangent = Vector3(1, 0, 0);
    verts.push_back(vert);

    vert.position = Vector3(-0.5, -0.5, 0);
    vert.uvs[0] = Vector3(0, 1, 0);
    vert.normal = Vector3(0, 0, -1);
    vert.tangent = Vector3(0, 1, 0);
    vert.bitangent = Vector3(1, 0, 0);
    verts.push_back(vert);
    inds.push_back(0);
    inds.push_back(1);
    inds.push_back(2);
    inds.push_back(0);
    inds.push_back(2);
    inds.push_back(3);
    mesh->Init(verts, inds);

    mesh->CalculateBound();
    mesh->SetName("quad");

    return mesh;
}

std::shared_ptr<Mesh> MeshSample::CreateTerrain(std::vector<unsigned short> arrayData, int xCount, int yCount, int size,
                                                int height)
{
    /*
    m_nWidth = nWidth;
    m_nLength = nLength;

    BYTE* pRawImagePixels = new BYTE[m_nWidth * m_nLength];

    HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
    DWORD dwBytesRead;
    ::ReadFile(hFile, pRawImagePixels, (xCount * yCount), &dwBytesRead, NULL);
    
    ::CloseHandle(hFile);
    */
    return nullptr;
}
