#pragma once
#include "ISoundSystem.h"
#include <memory>

namespace dae
{
    class SdlSoundSystem final : public ISoundSystem {
    public:
        SdlSoundSystem();
        ~SdlSoundSystem() override;

        virtual void Play(const sound_id id, const float volume) override;
        virtual void Stop(const sound_id id) override;
        virtual void Load(sound_id id, const std::string& path) override;
        virtual void Mute(const sound_id id) override;
        virtual void GeneralMuteToggle() override;
        
        void MuteAllToggle();

        SdlSoundSystem(const SdlSoundSystem& other) = delete;
        SdlSoundSystem(SdlSoundSystem&& other) = delete;
        SdlSoundSystem& operator=(const SdlSoundSystem& other) = delete;
        SdlSoundSystem& operator=(SdlSoundSystem&& other) = delete;

    private:
        class SdlSoundSystemImpl; // Pimpl
        std::unique_ptr<SdlSoundSystemImpl> pImpl;
    };

    // Decorator Pattern -- Logging Methods
    class LoggingSoundSystem final : public ISoundSystem {
    public:
        LoggingSoundSystem(std::unique_ptr<ISoundSystem>&& ss) : _real_ss(std::move(ss)) {}

        void Play(const sound_id id, const float volume) override;
        void Load(sound_id id, const std::string& path) override;
        void Stop(sound_id id) override;
        void Mute(sound_id id) override;

    private:
        std::unique_ptr<ISoundSystem> _real_ss;
    };
}