#pragma once

#include <stdafx.h>

namespace dxe
{
    enum class VertexProp
    {
        pos,
        normal,
        tangent,
        binormal,
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
    };


    typedef struct VertexPropInfo
    {
    public:
        int size;
        int byteSize;
        int offset;
        int byteOffset;
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
		Vector3 position;
		Vector3 normal;
		Vector3 tangent;
        Vector3 binormal;
		Vector4 color;
		std::vector<Vector3> uvs;
		Vector4 boneId;
        Vector4 boneWeight;


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


