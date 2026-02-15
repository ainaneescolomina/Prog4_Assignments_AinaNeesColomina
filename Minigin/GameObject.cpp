#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"
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
	//const auto& pos = m_transform.GetPosition();
	//Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
	for (auto& component : m_components)
	{
		component->Render();
	}
}

//void dae::GameObject::SetTexture(const std::string& filename)
//{
//	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
//}

void dae::GameObject::SetPosition(float x, float y)
{
	m_transform.SetPosition(x, y, 0.0f);
}