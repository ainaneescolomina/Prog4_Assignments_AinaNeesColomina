#pragma once
#include "ISoundSystem.h"
#include <memory>

namespace dae
{
    class SdlSoundSystem final : public ISoundSystem {
    public:
        SdlSoundSystem();
        ~SdlSoundSystem() override;

        virtual void play(const sound_id id, const float volume) override;
        virtual void load(sound_id id, const std::string& path) override;

        SdlSoundSystem(const SdlSoundSystem& other) = delete;
        SdlSoundSystem(SdlSoundSystem&& other) = delete;
        SdlSoundSystem& operator=(const SdlSoundSystem& other) = delete;
        SdlSoundSystem& operator=(SdlSoundSystem&& other) = delete;

    private:
        class SdlSoundSystemImpl; // Pimpl
        std::unique_ptr<SdlSoundSystemImpl> pImpl;
    };

    class LoggingSoundSystem final : public ISoundSystem {
    public:
        LoggingSoundSystem(std::unique_ptr<ISoundSystem>&& ss) : _real_ss(std::move(ss)) {}

        void play(const sound_id id, const float volume) override;
        void load(sound_id id, const std::string& path) override;

    private:
        std::unique_ptr<ISoundSystem> _real_ss;
    };
}