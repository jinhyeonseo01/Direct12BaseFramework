#pragma once

#include <simple_mesh_LH.h>

namespace DirectX
{
    namespace SimpleMath
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

        inline Vector3 QuaternionToEuler(const Quaternion& quaternion) {
            // ���ʹϾ��� ȸ�� ��ķ� ��ȯ
            XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quaternion);

            // ��Ŀ��� ���Ϸ� �� ����
            float pitch, yaw, roll;

            // ��Ŀ��� ���Ϸ� �� ����
            pitch = std::asin(-rotationMatrix.r[2].m128_f32[0]);

            if (std::cos(pitch) > 0.0001) { // ����(Gimbal Lock) üũ
                yaw = std::atan2(rotationMatrix.r[2].m128_f32[1], rotationMatrix.r[2].m128_f32[2]);
                roll = std::atan2(rotationMatrix.r[1].m128_f32[0], rotationMatrix.r[0].m128_f32[0]);
            }
            else {
                yaw = std::atan2(-rotationMatrix.r[0].m128_f32[2], rotationMatrix.r[1].m128_f32[1]);
                roll = 0.0f;
            }

            return Vector3(pitch, yaw, roll);
        }
    }
}