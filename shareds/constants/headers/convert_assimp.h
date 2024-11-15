#pragma once

#include <assimp/color4.h>
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <assimp/types.h>
#include <assimp/vector2.h>
#include <assimp/color4.h>
#include <assimp/types.h>

#include "simple_mesh_LH.h"

class convert_assimp
{
public:
    static DirectX::SimpleMath::Matrix Format(const aiMatrix4x4& from);
    static DirectX::SimpleMath::Matrix Format(const aiMatrix3x3& from);
    static DirectX::SimpleMath::Vector3 Format(const aiVector3D& from);
    static DirectX::SimpleMath::Vector2 Format(const aiVector2D& from);
    static DirectX::SimpleMath::Quaternion Format(const aiQuaternion& pOrientation);
    static DirectX::SimpleMath::Color Format(const aiColor4D& color);
    static DirectX::SimpleMath::Ray Format(const aiRay& ray);
    static DirectX::SimpleMath::Plane Format(const aiPlane& plane);
};
