#include "Bone.h"

Bone::Bone()
{
}

Bone::Bone(const std::wstring& boneName, int boneId)
{
    this->boneName = boneName;
    this->boneId = boneId;
}

Bone::~Bone()
{

}
