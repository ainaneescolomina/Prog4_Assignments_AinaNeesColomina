#pragma once
#include "Observer.h"
#include <algorithm>
#include <vector>

namespace dae
{
    class Subject
    {
    public:
        explicit Subject(void* owner) : m_pOwner(owner) {}

        void AddObserver(Observer* observer) {
            // code to add an observer
            m_observers.emplace_back(observer);
        }

        void RemoveObserver(Observer* observer) {
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
        void* m_pOwner;
    };
}