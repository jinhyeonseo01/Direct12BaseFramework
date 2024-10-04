#pragma once

#include <simple_mesh_LH.h>

namespace DirectX
{
    namespace SimpleMath
    {
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
    }
}