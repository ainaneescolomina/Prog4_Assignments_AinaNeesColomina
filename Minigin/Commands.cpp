#include "Commands.h"
#include "GameObject.h"

dae::MoveCommand::MoveCommand(GameObject* obj, float inputX, float inputY)
	:m_pObject(obj), 
	m_inputX(inputX),
	m_inputY(inputY)
{

}

void dae::MoveCommand::Execute(float delta_time)
{
	if (!m_pObject) return;
	if (m_pObject->IsMarkedForDestroy()) return;
	auto pos = m_pObject->GetTransform().GetPosition();
	m_pObject->SetPosition(pos.x + (m_inputX * delta_time), pos.y + (m_inputY * delta_time));
}

void dae::ShootCommand::Execute(float)
{
	m_pObject->GetComponent<ShootComponent>()->Shoot();
}
