#pragma once
#include <Windows.h>
#include <Xinput.h>

namespace dae
{
    class Gamepad
    {
    public:
        Gamepad(int index);

        void Update();

        bool IsDown(WORD button) const;
        bool IsPressed(WORD button) const;
        bool IsUp(WORD button) const;

    private:
        int m_Index;

        XINPUT_STATE m_CurrentState{};
        XINPUT_STATE m_PreviousState{};

        WORD m_ButtonsPressedThisFrame{};
        WORD m_ButtonsReleasedThisFrame{};
    };
}