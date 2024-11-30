#pragma once

#include <simple_mesh_LH.h>

namespace DirectX::SimpleMath
{
    inline bool Equals(const float& a, const float& b) noexcept
    {
        return std::abs(a - b) <= 1e-6f;
    }

    inline bool Equals(const double& a, const double& b) noexcept
    {
        return std::abs(a - b) <= 1e-14f;
    }

    inline bool Equals(const Vector3& a, const Vector3& b) noexcept
    {
        return (a - b).LengthSquared() <= 1e-6f;
    }

    inline bool Equals(const Vector2& a, const Vector2& b) noexcept
    {
        return (a - b).LengthSquared() <= 1e-6f;
    }

    inline void ToEuler(const Quaternion& quat, Vector3& euler) noexcept
    {
        float x = quat.x;
        float y = quat.y;
        float z = quat.z;
        float w = quat.w;

        // Yaw (Z�� ȸ��)
        float siny_cosp = 2 * (w * z + x * y);
        float cosy_cosp = 1 - 2 * (y * y + z * z);
        float yaw = std::atan2(siny_cosp, cosy_cosp); // ��ũź��Ʈ ���

        // Pitch (Y�� ȸ��)
        float sinp = 2 * (w * y - z * x);
        float pitch;
        if (std::abs(sinp) >= 1)
            pitch = std::copysign(XM_PI / 2, sinp); // Gimbal lock ó��
        else
            pitch = std::asin(sinp); // �Ϲ����� ����� Pitch ���

        // Roll (X�� ȸ��)
        float sinr_cosp = 2 * (w * x + y * z);
        float cosr_cosp = 1 - 2 * (x * x + y * y);
        float roll = std::atan2(sinr_cosp, cosr_cosp); // ��ũź��Ʈ ���

        // ���Ϸ� ���� Vector3�� ���� (Roll, Pitch, Yaw ������)
        euler = Vector3(roll, pitch, yaw); // ����: Roll, Pitch, Yaw
    };

    inline Vector3 QuaternionToEuler(const Quaternion& quaternion)
    {
        // ���ʹϾ��� ȸ�� ��ķ� ��ȯ
        XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quaternion);

        // ��Ŀ��� ���Ϸ� �� ����
        float pitch, yaw, roll;

        // ��Ŀ��� ���Ϸ� �� ����
        pitch = std::asin(-rotationMatrix.r[2].m128_f32[0]);

        if (std::cos(pitch) > 0.0001)
        {
            // ����(Gimbal Lock) üũ
            yaw = std::atan2(rotationMatrix.r[2].m128_f32[1], rotationMatrix.r[2].m128_f32[2]);
            roll = std::atan2(rotationMatrix.r[1].m128_f32[0], rotationMatrix.r[0].m128_f32[0]);
        }
        else
        {
            yaw = std::atan2(-rotationMatrix.r[0].m128_f32[2], rotationMatrix.r[1].m128_f32[1]);
            roll = 0.0f;
        }

        return Vector3(pitch, yaw, roll);
    }

    inline Quaternion CreateRotationZXY(float rollZ, float pitchX, float yawY)
    {
        // Z, X, Y �� �࿡ ���� ȸ�� ���ʹϾ� ����
        Quaternion rotationZ = Quaternion::CreateFromAxisAngle(Vector3::UnitZ, rollZ); // Z�� ȸ�� (Roll)
        Quaternion rotationX = Quaternion::CreateFromAxisAngle(Vector3::UnitX, pitchX); // X�� ȸ�� (Pitch)
        Quaternion rotationY = Quaternion::CreateFromAxisAngle(Vector3::UnitY, yawY); // Y�� ȸ�� (Yaw)

        // Z -> X -> Y ������ ���յ� ���� ȸ�� ���ʹϾ� ����
        Quaternion finalRotation = rotationY * rotationX * rotationZ;
        //Quaternion finalRotation = rotationZ * rotationX * rotationY;
        return finalRotation;
    }

    inline Quaternion CreateRotationZXY(const Vector3& euler)
    {
        return CreateRotationZXY(euler.z, euler.x, euler.y);
    }

    inline Quaternion LookToQuaternion(const Vector3& position, const Vector3& targetDirection,
                                       const Vector3& up = Vector3::UnitY)
    {
        // forward ���� ����ȭ
        Vector3 forward = targetDirection;
        forward.Normalize();

        // ������(right) ���� ���
        Vector3 right = up.Cross(forward);
        right.Normalize();

        // ���ο� up ���� ����
        Vector3 recalculatedUp = forward.Cross(right);

        // ȸ�� ��� ����
        Matrix lookToMatrix(
            right.x, right.y, right.z, 0.0f,
            recalculatedUp.x, recalculatedUp.y, recalculatedUp.z, 0.0f,
            forward.x, forward.y, forward.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        // ȸ�� ��Ŀ��� ���ʹϾ� ����
        return Quaternion::CreateFromRotationMatrix(lookToMatrix);
    }

    inline Matrix CreateViewportMatrix(Viewport view) {
        // Scale factors
        float scaleX = view.width / 2.0f;
        float scaleY = view.height / 2.0f;
        float scaleZ = view.maxDepth - view.minDepth;

        // Translation factors
        float offsetX = 0 + scaleX;//view.x
        float offsetY = 0 + scaleY;//view.y
        float offsetZ = view.minDepth;

        // Viewport matrix
        return Matrix(
            scaleX, 0.0f, 0.0f, 0.0f,  // X scale
            0.0f, -scaleY, 0.0f, 0.0f,  // Y scale (Y-axis inverted)
            0.0f, 0.0f, scaleZ, 0.0f,  // Z scale
            offsetX, offsetY, offsetZ, 1.0f   // Translation
        );
    }
}
