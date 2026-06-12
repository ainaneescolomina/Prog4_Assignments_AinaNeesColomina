#include "Subscription.h"
#include "Observer.h"
#include "Subject.h"

namespace dae
{
    Subscription::~Subscription()
    {
        Unsubscribe();
    }

    Subscription::Subscription(Subscription&& other) noexcept
        : m_subject(std::move(other.m_subject)) // Properly move the weak pointer container
        , m_observer(other.m_observer)
    {
        other.m_observer = nullptr;
    }

    Subscription& Subscription::operator=(Subscription&& other) noexcept
    {
        if (this != &other)
        {
            Unsubscribe();

            m_subject = std::move(other.m_subject);
            m_observer = other.m_observer;

            other.m_observer = nullptr;
        }

        return *this;
    }

    void Subscription::Unsubscribe()
    {
        // Tries to elevate the weak reference to a temporary shared reference
        // If the subject is dead, lock() safely returns nullptr instead of throwing a read exception
        if (auto sharedSubject = m_subject.lock())
        {
            if (m_observer)
                sharedSubject->RemoveObserver(m_observer);
        }

        m_observer = nullptr;
    }
}