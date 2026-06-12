#include"GameCommands.h"
#include "GameObject.h"
#include "GameComponents.h"
#include "EnemyComponents.h"
#include "ServiceLocator.h"
#include "LevelManager.h"

#include "GalagaDiveState.h"
#include "GalagaTractorBeamState.h"

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

void dae::VersusBossDiveCommand::Execute(float, float)
{
	if (!m_pObject) return;
	if (m_pObject->IsMarkedForDestroy()) return;

	auto stateComp = m_pObject->GetComponent<EnemyStateComponent>();
	if (stateComp && stateComp->GetCurrentState()->GetType() == dae::EnemyStateType::Idle)
	{
		stateComp->SetState(std::make_unique<GalagaDiveState>());
	}
}

void dae::VersusBossTractBeamCommand::Execute(float, float)
{
	if (!m_pObject) return;
	if (m_pObject->IsMarkedForDestroy()) return;

	auto stateComp = m_pObject->GetComponent<EnemyStateComponent>();
	if (stateComp && stateComp->GetCurrentState()->GetType() == dae::EnemyStateType::Idle)
	{
		stateComp->SetState(std::make_unique<GalagaTractorBeamState>());
	}
}
