#include "stdafx.h"

DirectX::SimpleMath::Matrix convert_assimp::Format(const aiMatrix4x4& mat)
{
    DirectX::SimpleMath::Matrix result(mat[0]);
    result._11 = mat.a1; result._21 = mat.a2; result._31 = mat.a3; result._41 = mat.a4;
    result._12 = mat.b1; result._22 = mat.b2; result._32 = mat.b3; result._42 = mat.b4;
    result._13 = mat.c1; result._23 = mat.c2; result._33 = mat.c3; result._43 = mat.c4;
    result._14 = mat.d1; result._24 = mat.d2; result._34 = mat.d3; result._44 = mat.d4;
    //Matrix result2(mat[0]);
    //result(mat[0]);
    //result = result.Transpose();
    return result;
}

DirectX::SimpleMath::Matrix convert_assimp::Format(const aiMatrix3x3& mat)
{
    DirectX::SimpleMath::Matrix result = DirectX::SimpleMath::Matrix::Identity;
    result._11 = mat.a1; result._21 = mat.a2; result._31 = mat.a3;
    result._12 = mat.b1; result._22 = mat.b2; result._32 = mat.b3;
    result._13 = mat.c1; result._23 = mat.c2; result._33 = mat.c3;
    return result;
}

DirectX::SimpleMath::Vector3 convert_assimp::Format(const aiVector3D& from)
{
    return DirectX::SimpleMath::Vector3(from.x, from.y, from.z);
}

DirectX::SimpleMath::Vector2 convert_assimp::Format(const aiVector2D& from)
{
    return DirectX::SimpleMath::Vector2(from.x, from.y);
}

DirectX::SimpleMath::Quaternion convert_assimp::Format(const aiQuaternion& pOrientation)
{
    return DirectX::SimpleMath::Quaternion(pOrientation.x, pOrientation.y, pOrientation.z, pOrientation.w);
}
DirectX::SimpleMath::Color convert_assimp::Format(const aiColor4D& color)
{
    return DirectX::SimpleMath::Color(color.r, color.g, color.b, color.a);
}
DirectX::SimpleMath::Ray convert_assimp::Format(const aiRay& ray)
{
    return DirectX::SimpleMath::Ray(Format(ray.pos), Format(ray.dir));
}
DirectX::SimpleMath::Plane convert_assimp::Format(const aiPlane& plane)
{
    return DirectX::SimpleMath::Plane(plane.a, plane.b, plane.c, plane.d);
}