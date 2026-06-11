#include "SdlSoundSystem.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>
#include <string>

namespace dae {

    struct PlayMessage
    {
        sound_id id;
        float volume;
    };

    class SdlSoundSystem::SdlSoundSystemImpl
    {
    public:
        SdlSoundSystemImpl() {
            // 1. Initialize SDL Audio Subsystem
            SDL_InitSubSystem(SDL_INIT_AUDIO);

            // 2. Initialize Mixer
            if (!MIX_Init()) {
                std::cerr << "MIX_Init failed: " << SDL_GetError() << std::endl;
                return;
            }

            // 3. Create the Mixer Device
            m_pMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
            if (!m_pMixer) {
                std::cerr << "Mixer creation failed: " << SDL_GetError() << std::endl;
                return;
            }

#ifndef __EMSCRIPTEN__
            m_Thread = std::thread(&SdlSoundSystemImpl::ProcessQueue, this);
#endif
        }

        ~SdlSoundSystemImpl() {
#ifndef __EMSCRIPTEN__
            m_Exit = true;
            m_Condition.notify_one();
            if (m_Thread.joinable()) m_Thread.join();
#endif
            for (auto& pair : m_AudioMap) {
                MIX_DestroyAudio(pair.second);
            }
            m_AudioMap.clear();

            if (m_pMixer)
            {
                MIX_DestroyMixer(m_pMixer);
                m_pMixer = nullptr;
            }

            //MIX_Quit();
            //SDL_QuitSubSystem(SDL_INIT_AUDIO);
        }

        void AddToQueue(sound_id id, float volume) {
#ifdef __EMSCRIPTEN__
            Play(id, volume);
#else
            // locks the mutex instantly and unlocks automatically when it goes out of scope
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Queue.push({ id, volume });
            m_Condition.notify_one();
#endif
        }

        void Load(sound_id id, const std::string& path)
        {
            if (!m_pMixer) return;

            // we lock here because asset loading can modify m_AudioMap (audio thread might be reading during Play lookups)
            std::lock_guard<std::mutex> lock(m_Mutex);
            if (m_AudioMap.contains(id)) return;

            MIX_Audio* audio = MIX_LoadAudio(m_pMixer, path.c_str(), true);

            if (!audio)
            {
                std::cerr << "Failed to load sound: " << path << "\n";
                return;
            }

            m_AudioMap[id] = audio;
        }

        void Stop(sound_id id)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);

            // Loop backward through active sounds to safely erase matches without skipping indices
            for (auto it = m_ActiveSounds.begin(); it != m_ActiveSounds.end();)
            {
                if (it->id == id)
                {
                    MIX_StopTrack(it->track, 0);    // Stop track with no delay
                    MIX_DestroyTrack(it->track);  // Free the track instance reference 
                    it = m_ActiveSounds.erase(it); // Erase from saved list
                }
                else
                {
                    ++it;
                }
            }
        }

        void Mute(sound_id id)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);

            for (auto& sound : m_ActiveSounds)
            {
                if (sound.id == id)
                {
                    sound.mute = !sound.mute; // Toggle individual mute state flag

                    // If global mute is active, force volume to 0. Otherwise apply individual mute rule
                    float targetVolume = (m_GlobalMute || sound.mute) ? 0.0f : sound.volume;
                    MIX_SetTrackGain(sound.track, targetVolume);
                }
            }
        }

        void MuteAllToggle()
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_GlobalMute = !m_GlobalMute; // Toggle global master mute flag

            for (auto& sound : m_ActiveSounds)
            {
                // If global mute turns on, silence everything. 
                // If it turns off, restore original volume only if this specific sound isn't individually muted.
                float targetVolume = (m_GlobalMute || sound.mute) ? 0.0f : sound.volume;
                MIX_SetTrackGain(sound.track, targetVolume);
            }
        }

    private:
        void ProcessQueue() {
            while (!m_Exit) {
                PlayMessage message;
                {
                    std::unique_lock<std::mutex> lock(m_Mutex);
                    m_Condition.wait(lock, [this] { return !m_Queue.empty() || m_Exit; });
                    if (m_Exit && m_Queue.empty()) return;
                    message = m_Queue.front();
                    m_Queue.pop();
                    lock.unlock();
                }
                Play(message.id, message.volume);
            }
        }

        void Play(sound_id id, float volume)
        {
            if (!m_pMixer) return;

            std::lock_guard<std::mutex> lock(m_Mutex);

            // We clean out done traks
            // If MIX_GetTrackAudio returns nullptr track is done
            for (auto it = m_ActiveSounds.begin(); it != m_ActiveSounds.end();)
            {
                if (MIX_GetTrackAudio(it->track) == nullptr)
                {
                    MIX_DestroyTrack(it->track);
                    it = m_ActiveSounds.erase(it);
                }
                else
                    ++it;
            }

            auto it = m_AudioMap.find(id);
            if (it == m_AudioMap.end())
            {
                std::cerr << "Sound not loaded: " << id << "\n";
                return;
            }

            MIX_Track* track = MIX_CreateTrack(m_pMixer);
            if (!track) return;

            ActiveSound newSound{ id, volume, track, false };

            MIX_SetTrackAudio(track, it->second);
            MIX_SetTrackGain(track, m_GlobalMute ? 0.0f : volume);
            MIX_PlayTrack(track, 0);

            // save to edit later
            m_ActiveSounds.push_back(newSound);
        }

        std::queue<PlayMessage> m_Queue;
        std::thread m_Thread;
        std::mutex m_Mutex;
        std::condition_variable m_Condition;
        bool m_Exit = false;

        MIX_Mixer* m_pMixer = nullptr;
        std::unordered_map<sound_id, MIX_Audio*> m_AudioMap;

        struct ActiveSound
        {
            sound_id id;
            float volume;
            MIX_Track* track;
            bool mute;
        };

        std::vector<ActiveSound> m_ActiveSounds;
        bool m_GlobalMute = false;
    };

    // SdlSoundSystem Methods
    SdlSoundSystem::SdlSoundSystem() : pImpl(std::make_unique<SdlSoundSystemImpl>()) {}
    SdlSoundSystem::~SdlSoundSystem() = default;

    void SdlSoundSystem::Play(const sound_id id, const float volume) 
    {
        pImpl->AddToQueue(id, volume);
    }

    void SdlSoundSystem::Load(const sound_id id, const std::string& path)
    {
        pImpl->Load(id, path);
    }

    void dae::SdlSoundSystem::Stop(const sound_id id)
    {
        pImpl->Stop(id);
    }

    void dae::SdlSoundSystem::Mute(const sound_id id)
    {
        pImpl->Mute(id);
    }

    void SdlSoundSystem::GeneralMuteToggle()
    {
        pImpl->MuteAllToggle();
    }

    void dae::SdlSoundSystem::MuteAllToggle()
    {
        pImpl->MuteAllToggle();
    }

    // Logging Methods
    void LoggingSoundSystem::Play(const sound_id id, const float volume) 
    {
        _real_ss->Play(id, volume);
        std::cout << "Playing sound " << id << " at volume " << volume << "\n";
    }
    void dae::LoggingSoundSystem::Load(sound_id id, const std::string& path)
    {
        _real_ss->Load(id, path);
        std::cout << "Loading sound " << id << "\n";
    }

    void dae::LoggingSoundSystem::Stop(const sound_id id)
    {
        _real_ss->Stop(id);
        std::cout << "Stopping sound " << id << "\n";
    }

    void dae::LoggingSoundSystem::Mute(const sound_id id)
    {
        _real_ss->Mute(id);
        std::cout << "Mute sound " << id << "\n";
    }
}