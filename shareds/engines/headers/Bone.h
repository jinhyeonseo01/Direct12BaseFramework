#pragma once
#include <stdafx.h>

namespace dxe
{
    class Bone : public std::enable_shared_from_this<Bone>
    {
    public:
        int boneId = 0;
        std::wstring boneName;

        Bone();
        Bone(const std::wstring& boneName, int boneId);
        virtual ~Bone();

    };
} 
