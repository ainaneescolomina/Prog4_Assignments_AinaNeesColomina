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

dae::DamageCommand::DamageCommand(GameObject* obj)
	:m_pObject(obj)
{
	
}

void dae::DamageCommand::Execute(float)
{
	m_pObject->GetComponent<dae::LivesComponent>()->TakeDamage(1);
}

dae::ScoreCommand::ScoreCommand(GameObject* obj, int score)
	:m_pObject(obj),
	m_score(score)
{
}

void dae::ScoreCommand::Execute(float)
{
	m_pObject->GetComponent<dae::ScoreComponent>()->AddScore(m_score);
}

void dae::ShootCommand::Execute(float)
{
	m_pObject->GetComponent<ShootComponent>()->Shoot();
}
