#pragma once
#include <memory>

namespace dae
{
    class Subject;
    class Observer;

    class Subscription final
    {
    public:
        // Accept a shared_ptr instead of a raw pointer to establish weak tracking safety bounds
        Subscription(std::shared_ptr<Subject> subject, Observer* observer)
            : m_subject(subject), m_observer(observer) {
        };

        ~Subscription();

        Subscription(const Subscription&) = delete;
        Subscription& operator=(const Subscription&) = delete;

        // Move constructors
        Subscription(Subscription&& other) noexcept;
        Subscription& operator=(Subscription&& other) noexcept;


    private:
        // REASONING: Uses a weak_ptr so that if the underlying Subject component is destroyed first, 
        // the subscription can safely detect that the memory is dead and prevent an access violation crash.
        std::weak_ptr<Subject> m_subject;
        Observer* m_observer{};

        void Unsubscribe();
    };
}