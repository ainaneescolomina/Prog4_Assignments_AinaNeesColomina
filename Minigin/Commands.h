#pragma once

namespace dae
{
	class Command
	{
	public:
		virtual ~Command() {}
		virtual void Execute(float delta_time) = 0;
	};
}