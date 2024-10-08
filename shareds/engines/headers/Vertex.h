#pragma once

#include <stdafx.h>

#include "IType.h"

namespace dxe
{
	class Vertex : public IType
	{
		Vector3 position;
		Vector3 normal;
		Vector3 tangent;
		Vector3 color;
		std::vector<Vector3> uvs;
		std::vector<uint16_t> boneId;
		std::vector<float> boneWeight;


		Vertex();
		virtual ~Vertex();

		Vertex(const Vertex& other);
		Vertex(Vertex&& other) noexcept;
		Vertex& operator=(const Vertex& other);
		Vertex& operator=(Vertex&& other) noexcept;

		bool operator==(const Vertex& other) const;

        void WriteBuffer(float* buffer, int& offset);
	};
}

