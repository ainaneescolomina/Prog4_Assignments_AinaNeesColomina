#pragma once
#include <memory>

namespace dae
{
    constexpr unsigned int GAMEPAD_DPAD_UP = 0x01;
    constexpr unsigned int GAMEPAD_DPAD_DOWN = 0x02;
    constexpr unsigned int GAMEPAD_DPAD_LEFT = 0x04;
    constexpr unsigned int GAMEPAD_DPAD_RIGHT = 0x08;

    class Gamepad
    {
    public:
        Gamepad(int index);
        ~Gamepad();

        Gamepad(Gamepad&&) noexcept;
        Gamepad& operator=(Gamepad&&) noexcept;

        void Update();

        bool IsDown(unsigned int button) const;
        bool IsPressed(unsigned int button) const;
        bool IsUp(unsigned int button) const;

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
}
