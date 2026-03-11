#include "Commands.h"
#include "GameObject.h"

dae::MoveCommand::MoveCommand(GameObject* obj, float inputX, float inputY)
	:m_object(obj), 
	m_inputX(inputX),
	m_inputY(inputY)
{

}

void dae::MoveCommand::Execute()
{
	auto pos = m_object->GetTransform().GetPosition();
	m_object->SetPosition(pos.x + m_inputX, pos.y + m_inputY);
}
