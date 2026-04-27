
#include "SoundSystem.h"
#include <memory>

namespace dae
{
    class null_sound_system final : public sound_system {
        void play(const sound_id, const float) override {}
    };

    class servicelocator final {
        static std::unique_ptr<sound_system> _ss_instance;

    public:
        static sound_system& get_sound_system() { return *_ss_instance; }

        static void register_sound_system(std::unique_ptr<sound_system>&& ss) {
            _ss_instance = ss == nullptr ? std::make_unique<null_sound_system>() : std::move(ss);
        }
    };

    std::unique_ptr<sound_system> servicelocator::_ss_instance{ std::make_unique<null_sound_system>() };
}