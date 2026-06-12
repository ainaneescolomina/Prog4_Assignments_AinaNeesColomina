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
        : m_subject(other.m_subject)
        , m_observer(other.m_observer)
        , m_isSubjectAlive(std::move(other.m_isSubjectAlive)) // Move token container
    {
        other.m_subject = nullptr;
        other.m_observer = nullptr;
    }

    Subscription& Subscription::operator=(Subscription&& other) noexcept
    {
        if (this != &other)
        {
            Unsubscribe();

            m_subject = other.m_subject;
            m_observer = other.m_observer;
            m_isSubjectAlive = std::move(other.m_isSubjectAlive); // Move token container

            other.m_subject = nullptr;
            other.m_observer = nullptr;
        }

        return *this;
    }

    void Subscription::Unsubscribe()
    {
        // 1. Try to lock the weak pointer. If the shared_ptr tracking block is gone, it returns nullptr.
        if (auto aliveToken = m_isSubjectAlive.lock())
        {
            // 2. If the block exists, verify that the Subject hasn't flipped it to false inside its destructor.
            if (*aliveToken && m_subject && m_observer)
            {
                m_subject->RemoveObserver(m_observer);
            }
        }

        m_subject = nullptr;
        m_observer = nullptr;
    }
}