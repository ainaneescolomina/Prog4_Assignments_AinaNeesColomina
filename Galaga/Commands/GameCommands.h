#include "Commands.h"

namespace dae
{
	class ShootCommand final : public Command
	{
	public:
		ShootCommand(GameObject* obj) : m_pObject(obj) {}

		void Execute(float delta_time) override;

	private:
		GameObject* m_pObject;
	};
}