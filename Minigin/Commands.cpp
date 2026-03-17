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
	auto pos = m_pObject->GetTransform().GetPosition();
	m_pObject->SetPosition(pos.x + (m_inputX * delta_time), pos.y + (m_inputY * delta_time));
}

dae::ShootCommand::ShootCommand(GameObject* obj, float inputX, float inputY)
	:m_pObject(obj),
	m_inputX(inputX),
	m_inputY(inputY)
{
	
}

void dae::ShootCommand::Execute(float delta_time)
{
	(void)delta_time;
	m_pObject->GetComponent<dae::LivesComponent>()->TakeDamage(1);
	m_pObject->GetComponent<dae::ScoreComponent>()->AddScore(100);
}
