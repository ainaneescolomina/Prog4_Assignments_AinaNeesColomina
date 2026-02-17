#include "GameObject.h"
#include "Components.h"

dae::GameObject::~GameObject() = default;

void dae::GameObject::Update(float delta_time)
{
	for (auto& component : m_components)
	{
		component->Update(delta_time);
	}
}

void dae::GameObject::Render() const
{
	for (auto& component : m_components)
	{
		component->Render();
	}
}

void dae::GameObject::SetPosition(float x, float y)
{
	m_transform.SetPosition(x, y, 0.0f);
}