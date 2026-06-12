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

            other.m_subject = nullptr;
            other.m_observer = nullptr;
        }

        return *this;
    }

    void Subscription::Unsubscribe()
    {
        if (m_subject && m_observer)
            m_subject->RemoveObserver(m_observer);
        m_subject = nullptr;
        m_observer = nullptr;
    }
}