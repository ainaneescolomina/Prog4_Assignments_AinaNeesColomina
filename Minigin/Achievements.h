#pragma once
#include "Observer.h"
#include "Components.h"
#include <iostream>

#if USE_STEAMWORKS
#pragma warning (push)
#pragma warning (disable:4996)
#include <steam_api.h>
#pragma warning (pop)
#endif


namespace dae
{
	class WinOneGameAchievement : public Observer
	{
	public:
		void Notify([[maybe_unused]] Event event, [[maybe_unused]] void* sender) override
		{
			if (m_check && m_achieved) return;

#if USE_STEAMWORKS
			if (!m_check)
			{
				// Check steam if already aschived before this session
				SteamUserStats()->GetAchievement("ACH_WIN_ONE_GAME", &m_achieved);
				m_check = true;
			}

			// Check if already achieved
			if (m_achieved) return;
			// Check score event
			if (event.id != make_sdbm_hash("UpdateScore")) return;

			ScoreComponent* component = static_cast<ScoreComponent*>(sender);
			auto score = component->GetScore();

			if (score >= 500)
			{
				SteamUserStats()->SetAchievement("ACH_WIN_ONE_GAME");
				SteamUserStats()->StoreStats();
				m_achieved = true;
			}
#endif
		}

	private:
		bool m_check{ false };
		bool m_achieved{ false };
	};
}