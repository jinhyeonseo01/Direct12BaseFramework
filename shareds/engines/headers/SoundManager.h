#pragma once

#include <stdafx.h>

namespace dxe
{

    inline Vector3 Vector3ToFmod(FMOD_VECTOR vec3)
    {
        return Vector3(vec3.x, vec3.y, vec3.z);
    }
    inline FMOD_VECTOR Vector3ToFmod(Vector3 vec3)
    {
        return FMOD_VECTOR(vec3.x, vec3.y, vec3.z);
    }

    class SoundManager : public std::enable_shared_from_this<SoundManager>
    {
    public:
        SoundManager();
        virtual ~SoundManager();

        void Init();
        void Release();

        std::shared_ptr<FMOD::System> _system;
        std::shared_ptr<FMOD::ChannelGroup> _channelGroup;


        std::shared_ptr<FMOD::Sound> LoadSound(int id, const std::string& path);
        std::shared_ptr<FMOD::Sound> GetSound(int id);
        std::shared_ptr<FMOD::Channel> PlaySoundSFX(std::shared_ptr<FMOD::Sound> sound, bool isPlayNow = true);
        std::shared_ptr<FMOD::Channel> PlaySoundBGM(std::shared_ptr<FMOD::Sound> sound, bool isPlayNow = true);
        /// @brief 감쇠 설정
        /// @param minDistance 최대 볼륨 거리
        /// @param maxDistance 소리가 완전히 감쇠되는 거리
        /// @param innerAngle 모든 방향에서 최대 볼륨
        /// @param outerAngle 모든 방향에서 감쇠가 없음
        /// @param outerVolume 외부 각도에서 볼륨이 50%로 줄어듭니다.
        void SetSoundAtten(std::shared_ptr<FMOD::Sound>& sound, float minD=10, float maxD=100, float innerAngle=360, float outerAngle=360, float outerVolume = 1);
        /// @brief 리스너의 상태 세팅
        /// @param position
        /// @param velocity 
        /// @param forward 
        /// @param up 
        void SetListener3DState(const Vector3& position, const Vector3& velocity, const Vector3& forward, const Vector3& up);
        void SetSound3DState(std::shared_ptr<FMOD::Channel>& channel, const Vector3& position, const Vector3& velocity);
    };
}

