#pragma once

#include <assimp/color4.h>
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <assimp/types.h>
#include <assimp/vector2.h>

#include "simple_mesh_LH.h"

class convert_assimp
{
public:
    static inline DirectX::SimpleMath::Matrix Format(const aiMatrix4x4& from);
    static inline DirectX::SimpleMath::Matrix Format(const aiMatrix3x3& from);
    static inline DirectX::SimpleMath::Vector3 Format(const aiVector3D& from);
    static inline DirectX::SimpleMath::Vector2 Format(const aiVector2D& from);
    static inline DirectX::SimpleMath::Quaternion Format(const aiQuaternion& pOrientation);
    static inline DirectX::SimpleMath::Color Format(aiColor4D color);
    static inline DirectX::SimpleMath::Ray Format(aiRay ray);
    static inline DirectX::SimpleMath::Plane Format(aiPlane plane);
};