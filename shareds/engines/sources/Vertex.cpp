#include "Vertex.h"

Vertex::Vertex() : position(0,0,0), normal(0,0,1), tangent(0,1,0)
{
	uvs.reserve(2);
	boneId.reserve(4);
	boneWeight.reserve(4);
}

Vertex::~Vertex()
{
	uvs.clear();
	boneId.clear();
	boneWeight.clear();
}

Vertex::Vertex(const Vertex& other)
{
	position = other.position;
	normal = other.normal;
	tangent = other.tangent;
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
