#pragma once

namespace dae
{
	class Command
	{
	public:
		virtual ~Command() {}
		virtual void Execute(float delta_time, float value_modifier = 1.0f) = 0;
	};
}