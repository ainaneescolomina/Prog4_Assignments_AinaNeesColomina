#pragma once
#include "Observer.h"
#include <algorithm>
#include <vector>

namespace dae
{
    class Subject
    {
    public:

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

    protected:
        void NotifyObservers(Event event) {
            for (auto observer : m_observers)
                observer->Notify(event, this);
        }

    private:
        std::vector<Observer*> m_observers;
    };
}