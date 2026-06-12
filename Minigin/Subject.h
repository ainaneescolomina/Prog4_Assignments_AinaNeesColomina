#pragma once
#include "Observer.h"
#include <algorithm>
#include <vector>
#include <memory>

namespace dae
{
    class Subject final
    {
    public:

        explicit Subject(void* owner)
            : m_pOwner(owner)
            , m_isAlive(std::make_shared<bool>(true))
        {
        }
        
        ~Subject()
        {
            // Inform all active subscriptions that this subject is dead!
            if (m_isAlive)
            {
                *m_isAlive = false;
            }
        }

        Subject(const Subject&) = delete;
        Subject(Subject&&) = delete;
        Subject& operator=(const Subject&) = delete;
        Subject& operator=(Subject&&) = delete;

        Subscription AddObserver(Observer* observer)
        {
            m_observers.emplace_back(observer);
            // Pass 'this' AND our state bool to the subscription
            return Subscription(this, m_isAlive, observer);
        }

        void RemoveObserver(Observer* observer) {
            if (m_observers.empty()) return;
            // code to remove an observer
            m_observers.erase(
                std::remove_if(m_observers.begin(), m_observers.end(),
                    [observer](auto obs)
                    {
                        return obs == observer;
                    }),
                m_observers.end());
        }

        // Send the owner data
        void NotifyObservers(Event event) {
            NotifyObservers(event, m_pOwner);
        }

        // Send the data of other (ex OnCollision)
        void NotifyObservers(Event event, void* senderOverride)
        {
            for (auto observer : m_observers)
                observer->Notify(event, senderOverride);
        }

    private:
        std::vector<Observer*> m_observers;
        void* m_pOwner{};

        std::shared_ptr<bool> m_isAlive; // Shared reference of subject state
    };
}