#pragma once
#include <string>

namespace dae
{
    using sound_id = unsigned short;

    class ISoundSystem {
    public:
        virtual ~ISoundSystem() = default;

        virtual void Play(const sound_id id, const float volume) = 0;
        virtual void Stop(const sound_id id) = 0;
        virtual void Load(sound_id id, const std::string& path) = 0;
        virtual void Mute(const sound_id id) = 0;
        virtual void GeneralMuteToggle() = 0;
    };
}