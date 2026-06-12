#pragma once

namespace dae
{
    class Subject;
    class Observer;

    class Subscription final
    {
    public:
        Subscription(Subject* subject, Observer* observer)
            : m_subject(subject), m_observer(observer) {
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

        void Unsubscribe();
    };
}