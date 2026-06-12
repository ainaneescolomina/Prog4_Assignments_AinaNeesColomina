#include"GameCommands.h"
#include "GameObject.h"
#include "GameComponents.h"
#include "ServiceLocator.h"
#include "LevelManager.h"

dae::MoveCommand::MoveCommand(GameObject* obj, float inputX, float inputY)
	:m_pObject(obj),
	m_inputX(inputX),
	m_inputY(inputY)
{

}

void dae::MoveCommand::Execute(float delta_time, float value_modifier)
{
	if (!m_pObject) return;
	if (m_pObject->IsMarkedForDestroy()) return;
	
	auto moveComp = m_pObject->GetComponent<dae::MovementComponent>();
	if (moveComp && moveComp->GetIsFrozen()) return;

	float customInputX = m_inputX * value_modifier;
	auto pos = m_pObject->GetTransform().GetPosition();
	m_pObject->SetPosition(pos.x + (customInputX * delta_time), pos.y + (m_inputY * delta_time));
}

void dae::ShootCommand::Execute(float, float)
{
	m_pObject->GetComponent<ShootComponent>()->Shoot();
}

void dae::MuteToggleCommand::Execute(float, float)
{
	auto& sound = dae::servicelocator::get_sound_system();
	sound.GeneralMuteToggle();
}

void dae::SkipLevelCommand::Execute(float, float)
{
	if (m_pLevelManager) m_pLevelManager->SkipLevel();
}