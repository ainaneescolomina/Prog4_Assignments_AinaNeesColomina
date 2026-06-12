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

	class LevelManager;
	class SkipLevelCommand final : public Command
	{
	public:
		explicit SkipLevelCommand(LevelManager* levelManager) : m_pLevelManager(levelManager) {}

		void Execute(float delta_time, float valueModifier = 1.0f) override;

	private:
		LevelManager* m_pLevelManager;
	};

	// VERSUS MODE
	class VersusBossDiveCommand final : public Command
	{
	public:
		explicit VersusBossDiveCommand(GameObject* obj) : m_pObject(obj) {}
		void Execute(float delta_time, float valueModifier = 1.0f) override;
	private:
		GameObject* m_pObject;
	};

	class VersusBossTractBeamCommand final : public Command
	{
	public:
		explicit VersusBossTractBeamCommand(GameObject* obj) : m_pObject(obj) {}
		void Execute(float delta_time, float valueModifier = 1.0f) override;
	private:
		GameObject* m_pObject;
	};
}