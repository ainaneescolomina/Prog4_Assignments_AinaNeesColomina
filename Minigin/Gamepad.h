#pragma once
#include <memory>

namespace dae
{
    constexpr unsigned int GAMEPAD_DPAD_UP = 0x01;
    constexpr unsigned int GAMEPAD_DPAD_DOWN = 0x02;
    constexpr unsigned int GAMEPAD_DPAD_LEFT = 0x04;
    constexpr unsigned int GAMEPAD_DPAD_RIGHT = 0x08;
    constexpr unsigned int GAMEPAD_START = 0x10;
    constexpr unsigned int GAMEPAD_BACK = 0x20;
    constexpr unsigned int GAMEPAD_LEFT_THUMB = 0x40;
    constexpr unsigned int GAMEPAD_RIGHT_THUMB = 0x80;
    constexpr unsigned int GAMEPAD_LEFT_SHOULDER = 0x100;
    constexpr unsigned int GAMEPAD_RIGHT_SHOULDER = 0x200;
    constexpr unsigned int GAMEPAD_A = 0x1000;
    constexpr unsigned int GAMEPAD_B = 0x2000;
    constexpr unsigned int GAMEPAD_X = 0x4000;
    constexpr unsigned int GAMEPAD_Y = 0x8000;

    class Gamepad final
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
        std::unique_ptr<Impl> m_pImpl;
    };
}
