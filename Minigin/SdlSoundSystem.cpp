#include "SdlSoundSystem.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace dae {

    struct PlayMessage
    {
        sound_id id;
        float volume;
    };

    class sdl_sound_system::Impl 
    {
    public:
        Impl()
        {
#ifndef __EMSCRIPTEN__
            m_Thread = std::thread(&Impl::ProcessQueue, this);
#endif
        }

        ~Impl() 
        {
#ifndef __EMSCRIPTEN__
            m_Exit = true;
            m_Condition.notify_one();
#endif
            if (m_Thread.joinable()) m_Thread.join();
        }

        void AddToQueue(sound_id id, float volume) 
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Queue.push({ id, volume });
            m_Condition.notify_one();
        }

    private:
        void ProcessQueue() 
        {
            while (!m_Exit) 
            {
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
            // Place your SDL_mixer load and play logic here [cite: 221]
        }

        std::queue<PlayMessage> m_Queue;
        std::thread m_Thread;
        std::mutex m_Mutex;
        std::condition_variable m_Condition;
        bool m_Exit = false;
    };

    // --- SdlSoundSystem Methods ---

    sdl_sound_system::sdl_sound_system() : pImpl(std::make_unique<Impl>()) {}
    sdl_sound_system::~sdl_sound_system() = default;

    void sdl_sound_system::play(const sound_id id, const float volume) 
    {
        pImpl->AddToQueue(id, volume);
    }

    // --- Logging Methods ---

    void logging_sound_system::play(const sound_id id, const float volume) 
    {
        _real_ss->play(id, volume);
        std::cout << "Playing sound " << id << " at volume " << volume << "\n";
    }
}