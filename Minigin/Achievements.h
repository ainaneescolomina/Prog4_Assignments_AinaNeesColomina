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
		void Notify([[maybe_unused]] Event event, [[maybe_unused]] Subject* sender) override
		{
#if USE_STEAMWORKS
			// Check score event
			if (event.id == make_sdbm_hash("UpdateScore"))
			{
				ScoreComponent* component = static_cast<ScoreComponent*>(sender);
				auto score = component->GetScore();

				if (score >= 500)
				{
					SteamUserStats()->SetAchievement("ACH_WIN_ONE_GAME");
					SteamUserStats()->StoreStats();
				}
			}
#endif
		}
	};
}