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
        /// @brief ���� ����
        /// @param minDistance �ִ� ���� �Ÿ�
        /// @param maxDistance �Ҹ��� ������ ����Ǵ� �Ÿ�
        /// @param innerAngle ��� ���⿡�� �ִ� ����
        /// @param outerAngle ��� ���⿡�� ���谡 ����
        /// @param outerVolume �ܺ� �������� ������ 50%�� �پ��ϴ�.
        void SetSoundAtten(std::shared_ptr<FMOD::Sound>& sound, float minD=10, float maxD=100, float innerAngle=360, float outerAngle=360, float outerVolume = 1);
        /// @brief �������� ���� ����
        /// @param position
        /// @param velocity 
        /// @param forward 
        /// @param up 
        void SetListener3DState(const Vector3& position, const Vector3& velocity, const Vector3& forward, const Vector3& up);
        void SetSound3DState(std::shared_ptr<FMOD::Channel>& channel, const Vector3& position, const Vector3& velocity);
    };
}

