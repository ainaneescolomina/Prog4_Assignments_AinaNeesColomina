#pragma once
#include <string>

namespace dae
{
    using sound_id = unsigned short;

    class sound_system {
    public:
        virtual ~sound_system() = default;

        virtual void play(const sound_id id, const float volume) = 0;
        virtual void load(sound_id id, const std::string& path) = 0;
    };
}