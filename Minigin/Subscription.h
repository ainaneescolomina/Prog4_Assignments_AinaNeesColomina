#pragma once
#include <memory>

namespace dae
{
    class Subject;
    class Observer;

    class Subscription final
    {
    public:
        // Get the raw subject and its safety bool
        Subscription(Subject* subject, std::weak_ptr<bool> aliveToken, Observer* observer)
            : m_subject(subject), m_isSubjectAlive(aliveToken), m_observer(observer) {
        };

        ~Subscription();

        Subscription(const Subscription&) = delete;
        Subscription& operator=(const Subscription&) = delete;

        // Move constructors
        Subscription(Subscription&& other) noexcept;
        Subscription& operator=(Subscription&& other) noexcept;


    private:
        Subject* m_subject{};
        Observer* m_observer{};

        // REASONING: Holds a weak token to check if the raw Subject memory is still alive
        // before executing RAII unbinding operations in Unsubscribe().
        std::weak_ptr<bool> m_isSubjectAlive;

        void Unsubscribe();
    };
}