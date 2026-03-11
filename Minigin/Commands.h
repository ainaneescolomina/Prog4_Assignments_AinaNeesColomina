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

}