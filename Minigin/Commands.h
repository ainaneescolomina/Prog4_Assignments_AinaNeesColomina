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

	class ShootCommand final : public Command
	{
	public:
		ShootCommand(GameObject* obj) : m_pObject(obj) {}

		void Execute(float delta_time) override;

	private:
		GameObject* m_pObject;
	};

}