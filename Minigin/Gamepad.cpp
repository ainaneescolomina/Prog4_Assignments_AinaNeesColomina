#include "Gamepad.h"


dae::Gamepad::Gamepad(int index)
    :m_Index{index}
{
}

void dae::Gamepad::Update()
{
    m_PreviousState = m_CurrentState;

    ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
    XInputGetState(m_Index, &m_CurrentState);

    WORD buttonChanges = m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons;
    m_ButtonsPressedThisFrame = buttonChanges & m_CurrentState.Gamepad.wButtons;
    m_ButtonsReleasedThisFrame = buttonChanges & (~m_CurrentState.Gamepad.wButtons);
}

bool dae::Gamepad::IsUp(WORD button) const
{
    return m_ButtonsReleasedThisFrame & button;
}

bool dae::Gamepad::IsPressed(WORD button) const
{
    return m_CurrentState.Gamepad.wButtons & button;
}

bool dae::Gamepad::IsDown(WORD button) const
{
    return m_ButtonsPressedThisFrame & button;
}
