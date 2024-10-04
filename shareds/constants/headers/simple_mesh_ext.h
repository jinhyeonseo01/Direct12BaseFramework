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

            // Yaw (Z축 회전)
            float siny_cosp = 2 * (w * z + x * y);
            float cosy_cosp = 1 - 2 * (y * y + z * z);
            float yaw = std::atan2(siny_cosp, cosy_cosp); // 아크탄젠트 계산

            // Pitch (Y축 회전)
            float sinp = 2 * (w * y - z * x);
            float pitch;
            if (std::abs(sinp) >= 1)
                pitch = std::copysign(XM_PI / 2, sinp); // Gimbal lock 처리
            else
                pitch = std::asin(sinp); // 일반적인 경우의 Pitch 계산

            // Roll (X축 회전)
            float sinr_cosp = 2 * (w * x + y * z);
            float cosr_cosp = 1 - 2 * (x * x + y * y);
            float roll = std::atan2(sinr_cosp, cosr_cosp); // 아크탄젠트 계산

            // 오일러 각을 Vector3로 저장 (Roll, Pitch, Yaw 순서로)
            euler = Vector3(roll, pitch, yaw); // 순서: Roll, Pitch, Yaw
		};
    }
}