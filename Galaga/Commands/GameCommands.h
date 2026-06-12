#include "Commands.h"

namespace dae
{
	class GameObject;
	class MoveCommand final : public Command
	{
	public:
		MoveCommand(GameObject* obj, float inputX, float inputY);

		void Execute(float delta_time, float value_modifier = 1.0f) override;

	private:
		GameObject* m_pObject;
		float m_inputX;
		float m_inputY;
	};

	class ShootCommand final : public Command
	{
	public:
		ShootCommand(GameObject* obj) : m_pObject(obj) {}

		void Execute(float delta_time, float value_modifier = 1.0f) override;

	private:
		GameObject* m_pObject;
	};

	class MuteToggleCommand final : public Command
	{
	public:
		void Execute(float delta_time, float value_modifier = 1.0f) override;
	};
}