#pragma once
#include "ISoundSystem.h"
#include <memory>

namespace dae
{
    class NullSoundSystem final : public ISoundSystem {
        void play(const sound_id, const float) override {}
        void load(sound_id, const std::string&) override {};
    };

    class servicelocator final {
        static std::unique_ptr<ISoundSystem> _ss_instance;

    public:
        static ISoundSystem& get_sound_system() { return *_ss_instance; }

        static void register_sound_system(std::unique_ptr<ISoundSystem>&& ss) {
            _ss_instance = ss == nullptr ? std::make_unique<NullSoundSystem>() : std::move(ss);
        }

        static void shutdown() {
            _ss_instance.reset();
        }
    };
}