#include "SdlSoundSystem.h"
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

    class sdl_sound_system::SdlSoundSystemImpl
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
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Queue.push({ id, volume });
            m_Condition.notify_one();
#endif
        }

        void Load(sound_id id, const std::string& path)
        {
            if (!m_pMixer) return;
            if (m_AudioMap.contains(id)) return;

            MIX_Audio* audio = MIX_LoadAudio(m_pMixer, path.c_str(), true);

            if (!audio)
            {
                std::cerr << "Failed to load sound: " << path << "\n";
                return;
            }

            m_AudioMap[id] = audio;
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
                }
                Play(message.id, message.volume);
            }
        }

        void Play(sound_id id, float volume)
        {
            if (!m_pMixer) return;

            auto it = m_AudioMap.find(id);
            if (it == m_AudioMap.end())
            {
                std::cerr << "Sound not loaded: " << id << "\n";
                return;
            }

            MIX_Track* track = MIX_CreateTrack(m_pMixer);
            if (!track) return;

            MIX_SetTrackAudio(track, it->second);
            MIX_SetTrackGain(track, volume);
            MIX_PlayTrack(track, 0);
        }

        std::queue<PlayMessage> m_Queue;
        std::thread m_Thread;
        std::mutex m_Mutex;
        std::condition_variable m_Condition;
        bool m_Exit = false;

        MIX_Mixer* m_pMixer = nullptr;
        std::unordered_map<sound_id, MIX_Audio*> m_AudioMap;
    };

    // --- SdlSoundSystem Methods ---

    sdl_sound_system::sdl_sound_system() : pImpl(std::make_unique<SdlSoundSystemImpl>()) {}
    sdl_sound_system::~sdl_sound_system() = default;

    void sdl_sound_system::play(const sound_id id, const float volume) 
    {
        pImpl->AddToQueue(id, volume);
    }

    void sdl_sound_system::load(const sound_id id, const std::string& path)
    {
        pImpl->Load(id, path);
    }

    // --- Logging Methods ---

    void logging_sound_system::play(const sound_id id, const float volume) 
    {
        _real_ss->play(id, volume);
        std::cout << "Playing sound " << id << " at volume " << volume << "\n";
    }
    void dae::logging_sound_system::load(sound_id id, const std::string& path)
    {
        _real_ss->load(id, path);
        std::cout << "Loading sound " << id << "\n";
    }
}