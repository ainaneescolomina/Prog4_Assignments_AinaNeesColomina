#include"GameCommands.h"
#include "GameObject.h"
#include "GameComponents.h"

void dae::ShootCommand::Execute(float)
{
	m_pObject->GetComponent<ShootComponent>()->Shoot();
}