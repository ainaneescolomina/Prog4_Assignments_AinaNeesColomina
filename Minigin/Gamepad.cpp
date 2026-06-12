#include "Gamepad.h"

#ifndef __EMSCRIPTEN__
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#endif

class dae::Gamepad::Impl
{
public:
    Impl([[maybe_unused]]int index)
#ifndef __EMSCRIPTEN__
        : m_index(index)
#endif
    {}


    void Update()
    {
#ifndef __EMSCRIPTEN__
        m_previousState = m_currentState;

        ZeroMemory(&m_currentState, sizeof(XINPUT_STATE));
        XInputGetState(m_index, &m_currentState);

        WORD buttonChanges = m_currentState.Gamepad.wButtons ^ m_previousState.Gamepad.wButtons;
        m_buttonsPressedThisFrame = buttonChanges & m_currentState.Gamepad.wButtons;
        m_buttonsReleasedThisFrame = buttonChanges & (~m_currentState.Gamepad.wButtons);
#endif
    }

    bool IsDown(unsigned int button) const
    {
#ifndef __EMSCRIPTEN__
        return m_buttonsPressedThisFrame & button;
#else
        (void)button;
        return false;
#endif
    }

    bool IsPressed(unsigned int button) const
    {
#ifndef __EMSCRIPTEN__
        return m_currentState.Gamepad.wButtons & button;
#else
        (void)button;
        return false;
#endif
    }

    bool IsUp(unsigned int button) const
    {
#ifndef __EMSCRIPTEN__
        return m_buttonsReleasedThisFrame & button;
#else
        (void)button;
        return false;
#endif
    }

    // Left Joystick value
    float GetLeftStickX() const
    {
        // get the raw x short value (-32768 to 32768) from the XInput state
        auto rawX = m_currentState.Gamepad.sThumbLX;

        // check for the deadzone values
        if (std::abs(rawX) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) return 0.f;
        
        // normalize the value to a range between -1.0f and 1.0f.
        // XInput negative values max out at -32768, while positive values max out at 32767.
        return rawX < 0 ? static_cast<float>(rawX) / 32768.f : static_cast<float>(rawX) / 32767.f;
    }

private:
#ifndef __EMSCRIPTEN__
    int m_index{};

    XINPUT_STATE m_currentState{};
    XINPUT_STATE m_previousState{};

    WORD m_buttonsPressedThisFrame{};
    WORD m_buttonsReleasedThisFrame{};
#endif
};

dae::Gamepad::Gamepad(int index)
    : m_pImpl(std::make_unique<Impl>(index))
{
}

dae::Gamepad::~Gamepad() = default;

dae::Gamepad::Gamepad(Gamepad&&) noexcept = default;
dae::Gamepad& dae::Gamepad::operator=(Gamepad&&) noexcept = default;

void dae::Gamepad::Update()
{
    m_pImpl->Update();
}

bool dae::Gamepad::IsDown(unsigned int button) const
{
    return m_pImpl->IsDown(button);
}

bool dae::Gamepad::IsPressed(unsigned int button) const
{
    return m_pImpl->IsPressed(button);
}

bool dae::Gamepad::IsUp(unsigned int button) const
{
    return m_pImpl->IsUp(button);
}

float dae::Gamepad::GetLeftStickX() const 
{ 
    return m_pImpl->GetLeftStickX(); 
}
