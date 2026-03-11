#include "Gamepad.h"

#ifndef __EMSCRIPTEN__
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#endif

class dae::Gamepad::Impl
{
public:
    Impl(int index)
        : m_Index(index)
    {}

    void Update()
    {
#ifndef __EMSCRIPTEN__
        m_PreviousState = m_CurrentState;

        ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
        XInputGetState(m_Index, &m_CurrentState);

        WORD buttonChanges = m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons;
        m_ButtonsPressedThisFrame = buttonChanges & m_CurrentState.Gamepad.wButtons;
        m_ButtonsReleasedThisFrame = buttonChanges & (~m_CurrentState.Gamepad.wButtons);
#endif
    }

    bool IsDown(unsigned int button) const
    {
#ifndef __EMSCRIPTEN__
        return m_ButtonsPressedThisFrame & button;
#else
        return false;
#endif
    }

    bool IsPressed(unsigned int button) const
    {
#ifndef __EMSCRIPTEN__
        return m_CurrentState.Gamepad.wButtons & button;
#else
        return false;
#endif
    }

    bool IsUp(unsigned int button) const
    {
#ifndef __EMSCRIPTEN__
        return m_ButtonsReleasedThisFrame & button;
#else
        return false;
#endif
    }

private:
#ifndef __EMSCRIPTEN__
    int m_Index{};

    XINPUT_STATE m_CurrentState{};
    XINPUT_STATE m_PreviousState{};

    WORD m_ButtonsPressedThisFrame{};
    WORD m_ButtonsReleasedThisFrame{};
#endif
};

dae::Gamepad::Gamepad(int index)
    : m_impl(std::make_unique<Impl>(index))
{
}

dae::Gamepad::~Gamepad() = default;

dae::Gamepad::Gamepad(Gamepad&&) noexcept = default;
dae::Gamepad& dae::Gamepad::operator=(Gamepad&&) noexcept = default;

void dae::Gamepad::Update()
{
    m_impl->Update();
}

bool dae::Gamepad::IsDown(unsigned int button) const
{
    return m_impl->IsDown(button);
}

bool dae::Gamepad::IsPressed(unsigned int button) const
{
    return m_impl->IsPressed(button);
}

bool dae::Gamepad::IsUp(unsigned int button) const
{
    return m_impl->IsUp(button);
}
