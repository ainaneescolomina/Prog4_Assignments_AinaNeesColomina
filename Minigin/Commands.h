#pragma once
#include <iostream>


namespace dae
{
	class Command
	{
	public:
		virtual ~Command() {}
		virtual void Execute() = 0;
	};

	class TestCommand : public Command
	{
	public:
		void Execute() override
		{
			std::cout << "Command executed\n";
		}
	};

	class GameObject;
	class MoveCommand final : public Command
	{
	public:
		MoveCommand(GameObject* obj, float inputX, float inputY);

		void Execute() override;

	private:
		GameObject* m_object;
		float m_inputX;
		float m_inputY;
	};

}