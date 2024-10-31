#include "stdafx.h"
#include "Bone.h"

Bone::Bone()
{
}

Bone::~Bone()
{

}

void Bone::SetName(const std::string& boneName, const std::string& nodeName)
{
    this->boneName = boneName;
    this->nodeName = nodeName;
}

void Bone::SetBoneID(int id)
{
    this->boneId = id;
}

void Bone::SetOffsetTransform(const Matrix& matrix)
{
    offsetTransform = matrix;
}
