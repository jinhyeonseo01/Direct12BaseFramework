#pragma once
#include <stdafx.h>

namespace dxe
{
    class Bone : public std::enable_shared_from_this<Bone>
    {
    public:
        Bone* _parent = nullptr;
        int boneId = -1;
        std::string boneName;
        std::string nodeName;
        Matrix offsetTransform = Matrix::Identity;

        Bone();
        void SetName(const std::string& boneName, const std::string& nodeName);
        void SetBoneID(int id);
        void SetOffsetTransform(const Matrix& matrix);
        virtual ~Bone();

    };
} 
