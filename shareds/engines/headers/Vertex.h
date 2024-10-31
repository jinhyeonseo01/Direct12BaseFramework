#pragma once

#include <stdafx.h>

namespace dxe
{
    enum class VertexProp
    {
        pos = 0,
        normal,
        tangent,
        bitangent,
        color,
        uv,
        uv0,
        uv1,
        uv2,
        uv3,
        uv4,
        uv5,
        uv6,
        uv7,
        bone_ids,
        bone_id0,
        bone_id1,
        bone_id2,
        bone_id3,
        bone_weights,
        bone_w0,
        bone_w1,
        bone_w2,
        bone_w3,
        COUNT
    };

    const std::array<std::string, static_cast<size_t>(VertexProp::COUNT)> PropNameStrings = {
        "POSITION",
        "NORMAL",
        "TANGENT",
        "BITANGENT",
        "COLOR",
        "TEXCOORD",
        "TEXCOORD",
        "TEXCOORD",
        "TEXCOORD",
        "TEXCOORD",
        "TEXCOORD",
        "TEXCOORD",
        "TEXCOORD",
        "TEXCOORD",
        "BONE_IDs",
        "BONE_ID",
        "BONE_ID",
        "BONE_ID",
        "BONE_ID",
        "BONE_Ws",
        "BONE_W",
        "BONE_W",
        "BONE_W",
        "BONE_W"
    };


    typedef struct VertexPropInfo
    {
    public:
        int size = 0;
        int byteSize = 0;
        int offset = 0;
        int byteOffset = 0;
        int index = 0;
        VertexProp prop;
    };
    class SelectorInfo
    {
    public:
        int totalSize = 0;
        int totalByteSize = 0;
        int propCount = 0;
        std::vector<VertexPropInfo> propInfos;
        std::vector<VertexProp> props;
    };

	class Vertex
	{
	public:
		Vector3 position = Vector3(0,0,0);
		Vector3 normal = Vector3(0, 0, 1);
		Vector3 tangent = Vector3(0, 1, 0);
        Vector3 bitangent = Vector3(0, 0, 0);
		Vector4 color = Vector4(1, 1, 0, 1);
        std::vector<Vector3> uvs{ 8 };
		Vector4 boneId = Vector4(-1, -1, -1, -1);
        Vector4 boneWeight = Vector4(0, 0, 0, 0);


		Vertex();
        Vertex(int uvCount, bool isStatic);
		virtual ~Vertex();

		Vertex(const Vertex& other);
		Vertex(Vertex&& other) noexcept;
		Vertex& operator=(const Vertex& other);
		Vertex& operator=(Vertex&& other) noexcept;

		bool operator==(const Vertex& other) const;

        void WriteBuffer(float* buffer, int& offset, const std::vector<VertexProp>& selector) const;

        static SelectorInfo GetSelectorInfo(const std::vector<VertexProp>& props);
	};
}


