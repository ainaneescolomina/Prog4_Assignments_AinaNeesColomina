#pragma once
#include "SoundSystem.h"
#include <memory>

namespace dae
{
    class sdl_sound_system final : public sound_system {
    public:
        sdl_sound_system();
        ~sdl_sound_system() override;

        void play(const sound_id id, const float volume) override;

        sdl_sound_system(const sdl_sound_system& other) = delete;
        sdl_sound_system(sdl_sound_system&& other) = delete;
        sdl_sound_system& operator=(const sdl_sound_system& other) = delete;
        sdl_sound_system& operator=(sdl_sound_system&& other) = delete;

    private:
        class Impl; // Pimpl
        std::unique_ptr<Impl> pImpl;
    };

    class logging_sound_system final : public sound_system {
    public:
        logging_sound_system(std::unique_ptr<sound_system>&& ss) : _real_ss(std::move(ss)) {}

        void play(const sound_id id, const float volume) override;

    private:
        std::unique_ptr<sound_system> _real_ss;
    };
}