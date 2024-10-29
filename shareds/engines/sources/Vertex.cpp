#include "Vertex.h"

namespace dxe
{
    Vertex::Vertex() : Vertex(2, false)
    {

    }

    Vertex::Vertex(int uvCount, bool isStatic) : position(0, 0, 0), normal(0, 0, 1), tangent(0, 1, 0)
    {
        uvs.reserve(uvCount);
    }

    Vertex::~Vertex()
    {
        uvs.clear();
    }

    Vertex::Vertex(const Vertex& other)
    {
        position = other.position;
        normal = other.normal;
        tangent = other.tangent;
        bitangent = other.bitangent;
        color = other.color;
        uvs = other.uvs;
        boneId = other.boneId;
        boneWeight = other.boneWeight;
    }

    Vertex::Vertex(Vertex&& other) noexcept
    {
        position = other.position;
        normal = other.normal;
        tangent = other.tangent;
        bitangent = other.bitangent;
        color = other.color;
        uvs = std::move(other.uvs);
        boneId = std::move(other.boneId);
        boneWeight = std::move(other.boneWeight);
    }

    Vertex& Vertex::operator=(const Vertex& other)
    {
        if (this == &other)
            return *this;
        position = other.position;
        normal = other.normal;
        tangent = other.tangent;
        bitangent = other.bitangent;
        color = other.color;
        uvs = other.uvs;
        boneId = other.boneId;
        boneWeight = other.boneWeight;
        return *this;
    }

    Vertex& Vertex::operator=(Vertex&& other) noexcept
    {
        if (this == &other)
            return *this;
        position = other.position;
        normal = other.normal;
        tangent = other.tangent;
        bitangent = other.bitangent;
        color = other.color;
        uvs = std::move(other.uvs);
        boneId = std::move(other.boneId);
        boneWeight = std::move(other.boneWeight);
        return *this;
    }

    bool Vertex::operator==(const Vertex& other) const
    {
        if ((uvs.size() == other.uvs.size()) && uvs.size() >= 1)
            return (uvs[0] == other.uvs[0]) && (position == other.position) && (normal == other.normal);
        return (position == other.position) && (normal == other.normal);
    }

    void Vertex::WriteBuffer(float* buffer, int& offset, const std::vector<VertexProp>& selector) const
    {
        size_t size = selector.size();
        size_t index = 0;
        while (index < size)
        {
            auto& select = selector[index];
            switch (select)
            {
            case VertexProp::pos:
                std::memcpy(&buffer[offset], &position, sizeof(Vector3));
                offset += 3;
                break;
            case VertexProp::normal:
                std::memcpy(&buffer[offset], &normal, sizeof(Vector3));
                offset += 3;
                break;
            case VertexProp::tangent:
                std::memcpy(&buffer[offset], &tangent, sizeof(Vector3));
                offset += 3;
                break;
            case VertexProp::bitangent:
                std::memcpy(&buffer[offset], &bitangent, sizeof(Vector3));
                offset += 3;
                break;
            case VertexProp::color:
                std::memcpy(&buffer[offset], &color, sizeof(Vector4));
                offset += 4;
                break;
            case VertexProp::uv:
            case VertexProp::uv0:
                std::memcpy(&buffer[offset], &uvs[0], sizeof(Vector3));
                offset += 3;
                break;
            case VertexProp::uv1:
                std::memcpy(&buffer[offset], &uvs[1], sizeof(Vector3));
                offset += 3;
                break;
            case VertexProp::uv2:
                std::memcpy(&buffer[offset], &uvs[2], sizeof(Vector3));
                offset += 3;
                break;
            case VertexProp::uv3:
                std::memcpy(&buffer[offset], &uvs[3], sizeof(Vector3));
                offset += 3;
                break;
            case VertexProp::uv4:
                std::memcpy(&buffer[offset], &uvs[4], sizeof(Vector3));
                offset += 3;
                break;
            case VertexProp::uv5:
                std::memcpy(&buffer[offset], &uvs[5], sizeof(Vector3));
                offset += 3;
                break;
            case VertexProp::uv6:
                std::memcpy(&buffer[offset], &uvs[6], sizeof(Vector3));
                offset += 3;
                break;
            case VertexProp::uv7:
                std::memcpy(&buffer[offset], &uvs[7], sizeof(Vector3));
                offset += 3;
                break;
            case VertexProp::bone_id0:
                std::memcpy(&buffer[offset++], &boneId.x, sizeof(float));
                break;
            case VertexProp::bone_id1:
                std::memcpy(&buffer[offset++], &boneId.y, sizeof(float));
                break;
            case VertexProp::bone_id2:
                std::memcpy(&buffer[offset++], &boneId.z, sizeof(float));
                break;
            case VertexProp::bone_id3:
                std::memcpy(&buffer[offset++], &boneId.w, sizeof(float));
                break;
            case VertexProp::bone_ids:
                std::memcpy(&buffer[offset], &boneId, sizeof(Vector4));
                offset += 4;
                break;
            case VertexProp::bone_w0:
                std::memcpy(&buffer[offset++], &boneWeight.x, sizeof(float));
                break;
            case VertexProp::bone_w1:
                std::memcpy(&buffer[offset++], &boneWeight.y, sizeof(float));
                break;
            case VertexProp::bone_w2:
                std::memcpy(&buffer[offset++], &boneWeight.z, sizeof(float));
                break;
            case VertexProp::bone_w3:
                std::memcpy(&buffer[offset++], &boneWeight.w, sizeof(float));
                break;
            case VertexProp::bone_weights:
                std::memcpy(&buffer[offset], &boneWeight, sizeof(Vector4));
                offset += 4;
                break;

            }
            ++index;
        }

    }

    SelectorInfo Vertex::GetSelectorInfo(const std::vector<VertexProp>& props)
    {
        SelectorInfo info;
        VertexPropInfo propInfo;
        info.props = props;


        int offset = 0;
        int byteOffset = 0;
        for (int i = 0; i < props.size(); i++)
        {
            auto& select = props[i];
            int elementTypeSize = sizeof(float);
            int byteSize = 0;
            int index = 0;
            switch (select)
            {
            case VertexProp::pos:
                byteSize = sizeof(Vector3);
                break;
            case VertexProp::normal:
                byteSize = sizeof(Vector3);
                break;
            case VertexProp::tangent:
                byteSize = sizeof(Vector3);
                break;
            case VertexProp::bitangent:
                byteSize = sizeof(Vector3);
                break;
            case VertexProp::color:
                byteSize = sizeof(Vector4);
                break;
            case VertexProp::uv:
            case VertexProp::uv0:
                byteSize = sizeof(Vector3);
                index = 0;
                break;
            case VertexProp::uv1:
                byteSize = sizeof(Vector3);
                index = 1;
                break;
            case VertexProp::uv2:
                byteSize = sizeof(Vector3);
                index = 2;
                break;
            case VertexProp::uv3:
                byteSize = sizeof(Vector3);
                index = 3;
                break;
            case VertexProp::uv4:
                byteSize = sizeof(Vector3);
                index = 4;
                break;
            case VertexProp::uv5:
                byteSize = sizeof(Vector3);
                index = 5;
                break;
            case VertexProp::uv6:
                byteSize = sizeof(Vector3);
                index = 6;
                break;
            case VertexProp::uv7:
                byteSize = sizeof(Vector3);
                index = 7;
                break;
            case VertexProp::bone_id0:
                byteSize = sizeof(float);
                index = 0;
                break;
            case VertexProp::bone_id1:
                byteSize = sizeof(float);
                index = 1;
                break;
            case VertexProp::bone_id2:
                byteSize = sizeof(float);
                index = 2;
                break;
            case VertexProp::bone_id3:
                byteSize = sizeof(float);
                index = 3;
                break;
            case VertexProp::bone_ids:
                byteSize = sizeof(Vector4);
                index = 0;
                break;
            case VertexProp::bone_w0:
                byteSize = sizeof(float);
                index = 0;
                break;
            case VertexProp::bone_w1:
                byteSize = sizeof(float);
                index = 1;
                break;
            case VertexProp::bone_w2:
                byteSize = sizeof(float);
                index = 2;
                break;
            case VertexProp::bone_w3:
                byteSize = sizeof(float);
                index = 3;
                break;
            case VertexProp::bone_weights:
                byteSize = sizeof(Vector4);
                index = 0;
                break;
            }
            int size = byteSize / elementTypeSize;

            propInfo.offset = offset;
            propInfo.byteOffset = byteOffset;
            propInfo.size = size;
            propInfo.byteSize = byteSize;
            propInfo.prop = select;
            propInfo.index = index;

            info.propInfos.push_back(propInfo);

            offset += size;
            byteOffset += byteSize;
        }
        info.totalByteSize = byteOffset;
        info.totalSize = offset;
        info.propCount = static_cast<int>(props.size());
        return info;
    }

}
