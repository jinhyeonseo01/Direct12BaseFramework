#include "SoundManager.h"

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}

void SoundManager::Init()
{
    {
        FMOD::System* system;
        FMOD::System_Create(&system);
        _system = std::shared_ptr<FMOD::System>(system);
    }
    _system->init(64, FMOD_INIT_NORMAL, nullptr);
}

void SoundManager::Release()
{
}

std::shared_ptr<FMOD::Sound> SoundManager::LoadSound(int id, const std::string& path)
{
}

std::shared_ptr<FMOD::Sound> SoundManager::GetSound(int id)
{

}

std::shared_ptr<FMOD::Channel> SoundManager::PlaySoundSFX(std::shared_ptr<FMOD::Sound> sound, bool isPlayNow)
{
    FMOD::Channel* channel;
    if (!sound) return nullptr;
    if (!_system) return nullptr;
    if (!_channelGroup) return nullptr;

    _system->playSound(sound.get(), _channelGroup.get(), isPlayNow, &channel);
    //channel->setVolume()

    return std::shared_ptr<FMOD::Channel>(channel);
}

std::shared_ptr<FMOD::Channel> SoundManager::PlaySoundBGM(std::shared_ptr<FMOD::Sound> sound, bool isPlayNow)
{
    FMOD::Channel* channel;
    if (!sound) return nullptr;
    if (!_system) return nullptr;
    if (!_channelGroup) return nullptr;

    _system->playSound(sound.get(), _channelGroup.get(), isPlayNow, &channel);
    //channel->setVolume()

    return std::shared_ptr<FMOD::Channel>(channel);
}

void SoundManager::SetSoundAtten(std::shared_ptr<FMOD::Sound>& sound, float minD, float maxD, float innerAngle,
    float outerAngle, float outerVolume)
{
    sound->set3DMinMaxDistance(minD, maxD);
    sound->set3DConeSettings(innerAngle, outerAngle, outerVolume);
}

void SoundManager::SetListener3DState(const Vector3& position, const Vector3& velocity, const Vector3& forward,
    const Vector3& up)
{
    FMOD_VECTOR pos = Vector3ToFmod(position);
    FMOD_VECTOR vel = Vector3ToFmod(velocity);
    FMOD_VECTOR f = Vector3ToFmod(forward);
    FMOD_VECTOR u = Vector3ToFmod(up);
    
    _system->set3DListenerAttributes(0, &pos, &vel, &f, &u);
}

void SoundManager::SetSound3DState(std::shared_ptr<FMOD::Channel>& channel, const Vector3& position, const Vector3& velocity)
{
    FMOD_VECTOR pos = Vector3ToFmod(position);
    FMOD_VECTOR vel = Vector3ToFmod(velocity);
    channel->set3DAttributes(&pos, &vel);
}
