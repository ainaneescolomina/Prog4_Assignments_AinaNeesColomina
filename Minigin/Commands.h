#pragma once
#include <iostream>


namespace dae
{
	class Command
	{
	public:
		virtual ~Command() {}
		virtual void Execute(float delta_time) = 0;
	};

	class GameObject;
	class MoveCommand final : public Command
	{
	public:
		MoveCommand(GameObject* obj, float inputX, float inputY);

		void Execute(float delta_time) override;

	private:
		GameObject* m_pObject;
		float m_inputX;
		float m_inputY;
	};

	class DamageCommand final : public Command
	{
	public:
		DamageCommand(GameObject* obj);

		void Execute(float delta_time) override;

	private:
		GameObject* m_pObject;
	};

	class ScoreCommand final : public Command
	{
	public:
		ScoreCommand(GameObject* obj, int score);

		void Execute(float delta_time) override;

	private:
		GameObject* m_pObject;
		int m_score;
	};

}